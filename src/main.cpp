#include <cstdio>
#include <mcpelauncher/zip_extractor.h>
#include <mcpelauncher/minecraft_extract_utils.h>

#define CLEAR_LINE "\033[2K\r"

int main(int argc, const char* argv[]) {
    if (argc != 3) {
        printf("mcpelauncher-extract <apk-file> <destination>");
        return 1;
    }

    std::string inPath = argv[1];
    std::string outPath = argv[2];

    ZipExtractor extractor (inPath);
    int lastPercentageReported = -1;
    printf("Collecting files to extract");
    extractor.extractTo(MinecraftExtractUtils::filterMinecraftFiles(outPath), [&lastPercentageReported]
            (size_t current, size_t max, ZipExtractor::FileHandle const& ent, size_t, size_t) {
        int percentage = (int) (current * 100 / max);
        if (percentage != lastPercentageReported) {
            printf(CLEAR_LINE "Extracting: %i%%", percentage);
            fflush(stdout);
        }
        lastPercentageReported = percentage;
    });
    printf(CLEAR_LINE "Done!\n");
    return 0;
}