#include <cstdio>
#include <mcpelauncher/zip_extractor.h>

#define CLEAR_LINE "\033[2K\r"

int main(int argc, const char* argv[]) {
    if (argc != 3) {
        printf("mcpelauncher-extract <apk-file> <destination>");
        return 1;
    }

    ZipExtractor extractor (argv[1]);
    std::string outPath = argv[2];
    int lastPercentageReported = -1;
    extractor.extractTo([outPath](const char* filename, std::string& outName) {
        outName = outPath + "/" + filename;
        return true;
    }, [&lastPercentageReported](size_t current, size_t max, ZipExtractor::FileHandle const& ent, size_t, size_t) {
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