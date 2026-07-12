#include <mcpelauncher/zip_extractor.h>
#include <cstring>
#include <vector>
#include <sys/stat.h>
#include <errno.h>

ZipExtractor::ZipExtractor(std::string const& path) {
    int err = 0;
    archive = zip_open(path.c_str(), 0, &err);
    if (archive == nullptr)
        throw ZipExtractionError("Failed to open zip " + std::to_string(err));
}

ZipExtractor::ZipExtractor(zip_source_t* source) {
    zip_error_t err;
    archive = zip_open_from_source(source, 0, &err);
    if (archive == nullptr) {
        zip_source_free(source);
        throw ZipExtractionError("Failed to open zip source " + std::string(zip_error_strerror(&err)) + " " + std::to_string(zip_error_code_zip(&err)));
    }
}

void ZipExtractor::mkdirRecursive(char* path, size_t len, bool createThisDir) {
    char* a = NULL;
    for (ssize_t i = len - 1; i >= 0; i--) {
        if (path[i] == '/') {
            a = (char*) &path[i];
            break;
        }
    }

    if (createThisDir && (mkdir(path, 0755) == 0 || errno == EEXIST))
        return;
    if (a != nullptr) {
        *a = '\0';
        mkdirRecursive(path, len, true);
        *a = '/';
    }
    if (createThisDir)
        mkdir(path, 0755);
}

void ZipExtractor::extractFile(zip_file* entry, std::string const& to, size_t fileSize,
                               std::function<void(size_t current, size_t max)> progress,
                               char* buf, size_t bufSize) {
    mkdirRecursive(to);

    FILE* out = fopen(to.c_str(), "wb");
    if (!out) {
        auto err = errno;
        auto strerr = err != 0 ? strerror(err) : "Unknown";
        throw ZipExtractionError("Failed to open file " + to + " for writing: " + (strerr ? strerr : "Unknown"));
    }

    progress(0, fileSize);
    zip_int64_t r;
    size_t total = 0;
    while ((r = zip_fread(entry, buf, bufSize)) > 0) {
        auto fr = fwrite(buf, sizeof(char), (size_t) r, out);
        if(fr != r) {
            auto err = errno;
            auto strerr = err != 0 ? strerror(err) : "Unknown";
            throw ZipExtractionError("Failed to write to file " + to + ": " + (strerr ? strerr : "Unknown"));
        }
        total += r;
        progress(total, fileSize);
    }
    progress(total, fileSize);
    if(fclose(out) != 0) {
        auto err = errno;
        auto strerr = err != 0 ? strerror(err) : "Unknown";
        throw ZipExtractionError("Failed to close file " + to + ": " + (strerr ? strerr : "Unknown"));
    }
}

void ZipExtractor::extractTo(std::vector<std::pair<zip_uint64_t, std::string>> const& files, size_t totalSize,
                             std::function<void (size_t current, size_t max, FileHandle const& entry,
                                                 size_t entryCurrent, size_t entryMax)> const& progress) {
    std::vector<char> vbuf(DEFAULT_BUF_SIZE);
    char* buf = vbuf.data();
    struct zip_stat zs;
    size_t current = 0;
    for (auto const& file : files) {
        if (zip_stat_index(archive, file.first, 0, &zs) != 0)
            throw ZipExtractionError("zip_stat_index failed");
        FileHandle handle (archive, file.first);
        if (handle.get() == nullptr)
            throw ZipExtractionError("Failed to open file in the zip");
        extractFile(handle, file.second, zs.size, [current, &handle, &progress, totalSize]
                (size_t fileCurrent, size_t fileMax) {
            progress(current + fileCurrent, totalSize, handle, fileCurrent, fileMax);
        }, buf, DEFAULT_BUF_SIZE);
        current += zs.size;
    }
}

void ZipExtractor::extractEntries(std::vector<EntryInfo> const& files,
                                  std::function<void (size_t current, size_t max, FileHandle const& entry,
                                                      size_t entryCurrent, size_t entryMax)> const& progress) {
    std::vector<std::pair<zip_uint64_t, std::string>> planned;
    size_t totalSize = 0;
    planned.reserve(files.size());
    for (auto const& file : files) {
        totalSize += (size_t) file.size;
        planned.emplace_back(file.index, file.outputName);
    }
    extractTo(planned, totalSize, progress);
}

std::vector<ZipExtractor::EntryInfo> ZipExtractor::listEntries(
        std::function<bool (const char* filename, std::string& outName)> const& filter) {
    std::vector<EntryInfo> files;
    zip_int64_t n = zip_get_num_entries(archive, 0);
    std::string filename;
    struct zip_stat zs;
    for (zip_int64_t i = 0; i < n; i++) {
        if (zip_stat_index(archive, (zip_uint64_t) i, 0, &zs) != 0)
            throw ZipExtractionError("zip_stat_index failed");
        if (filter(zs.name, filename)) {
            files.push_back({(zip_uint64_t) i, zs.name, filename, zs.size, zs.crc});
        }
    }
    return files;
}

size_t ZipExtractor::getFilteredSize(std::function<bool (const char* filename, std::string& outName)> const& filter) {
    size_t totalSize = 0;
    auto entries = listEntries(filter);
    for (auto const& entry : entries)
        totalSize += (size_t) entry.size;
    return totalSize;
}

void ZipExtractor::extractTo(std::function<bool (const char* filename, std::string& outName)> const& filter,
                             std::function<void (size_t current, size_t max, FileHandle const& entry,
                                                 size_t entryCurrent, size_t entryMax)> const& progress) {
    auto entries = listEntries(filter);
    return extractEntries(entries, progress);
}

std::vector<char> ZipExtractor::readFile(std::string const& filename) {
    struct zip_stat s;
    if (zip_stat(archive, filename.c_str(), 0, &s) != 0)
        throw ZipExtractionError("Failed to stat the specified file");
    FileHandle handle (zip_fopen(archive, filename.c_str(), 0));
    if (!handle)
        throw ZipExtractionError("Failed to open the specified file");
    std::vector<char> ret (s.size);
    for (size_t o = 0; o < s.size; ) {
        ssize_t r = zip_fread(handle, &ret.data()[o], s.size - o);
        if (r < 0)
            throw ZipExtractionError("Read failed");
        o += r;
    }
    return ret;
}
