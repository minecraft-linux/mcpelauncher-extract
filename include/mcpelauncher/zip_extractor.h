#pragma once

#include <string>
#include <functional>
#include <zip.h>
#include <vector>
#include <stdexcept>

struct ZipExtractionError : public std::runtime_error {
    explicit ZipExtractionError(std::string const& e) : std::runtime_error(e) {}
};

class ZipExtractor {

public:
    class FileHandle {

    private:
        zip_file* entry;

    public:
        FileHandle(zip_file* entry) : entry(entry) {}
        FileHandle(zip* archive, zip_uint64_t entry) : FileHandle(zip_fopen_index(archive, entry, 0)) {}
        ~FileHandle() { if (entry) zip_fclose(entry); }

        zip_file* get() { return entry; }
        operator zip_file*() { return entry; }

    };

private:
    static constexpr size_t DEFAULT_BUF_SIZE = 2 * 1024 * 1024;

    zip* archive;

    static void mkdirRecursive(char* path, size_t len, bool createThisDir = false);
    static void mkdirRecursive(std::string path, bool createThisDir = false) {
        mkdirRecursive(&path[0], path.size(), createThisDir);
    }

    static void extractFile(zip_file* entry, std::string const& to, size_t fileSize,
                            std::function<void (size_t current, size_t max)> progress,
                            char* buf, size_t bufSize);

    void extractTo(std::vector<std::pair<zip_uint64_t, std::string>> const& files, size_t totalSize,
                   std::function<void (size_t current, size_t max, FileHandle const& entry,
                                       size_t entryCurrent, size_t entryMax)> const& progress);

public:

    ZipExtractor(zip* archive) : archive(archive) {}
    ZipExtractor(std::string const& path);

    ~ZipExtractor() { zip_close(archive); }


    std::vector<char> readFile(std::string const& filename);

    void extractTo(std::function<bool (const char* filename, std::string& outName)> const& filter,
                   std::function<void (size_t current, size_t max, FileHandle const& entry,
                                       size_t entryCurrent, size_t entryMax)> const& progress);

};