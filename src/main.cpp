#include <cstdio>
#include <mcpelauncher/zip_extractor.h>
#include <mcpelauncher/minecraft_extract_utils.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#define CLEAR_LINE "\033[2K\r"

static void checkFolder(std::string const& path) {
    struct stat info;
    if (stat(path.c_str(), &info) < 0) {
        if (errno == ENOENT)
            return;
        printf("Warning: stat() failed on the specified destination\n");
        return;
    }
    if (!(info.st_mode & S_IFDIR)) {
        printf("The specified path exists and is not a directory\n");
        exit(1);
    }
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        printf("Failed to open the output directory\n");
        exit(1);
    }
    struct dirent* de;
    while ((de = readdir(dir)) != nullptr) {
        if (de->d_name[0] == '.')
            continue;
        printf("Warning: Directory is not empty\n");
        break;
    }
    closedir(dir);
}

int main(int argc, const char* argv[]) {
    if (argc != 3) {
        printf("mcpelauncher-extract <apk-file> <destination>\n");
        return 1;
    }

    std::string inPath = argv[1];
    std::string outPath = argv[2];

    checkFolder(outPath);

    ZipExtractor extractor (inPath);
    int lastPercentageReported = -1;
    printf("Collecting files to extract");
#if defined(__arm__) && (!defined(FORCE_ARM) || FORCE_ARM == 1 )
    std::string arch = "armeabi-v7a";
#else
    std::string arch = "x86_64";
#endif
    extractor.extractTo(MinecraftExtractUtils::filterMinecraftFiles(outPath, arch), [&lastPercentageReported]
            (size_t current, size_t max, ZipExtractor::FileHandle const& ent, size_t, size_t) {
        int percentage = (int) (current * 100 / max);
        if (percentage != lastPercentageReported) {
            printf(CLEAR_LINE "Extracting: %i%%", percentage);
            fflush(stdout);
        }
        lastPercentageReported = percentage;
    });
    printf(CLEAR_LINE "Done!\n");
    if (!MinecraftExtractUtils::checkMinecraftLibFile(outPath))
        printf("WARNING: libminecraftpe.so was not extracted. The specified APK is incompatible with the launcher.");
    return 0;
}
