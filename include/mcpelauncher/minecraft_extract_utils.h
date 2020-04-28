#pragma once

#include <string>
#include <functional>
#include <cstring>
#include <unistd.h>

class MinecraftExtractUtils {

public:
    static std::function<bool (const char* filename, std::string& outName)>
    filterMinecraftFiles(std::string const& dir) {
        if (!dir.empty() && dir[dir.length() - 1] != '/')
            return filterMinecraftFiles(dir + "/");
        using namespace std::placeholders;
        return std::bind(&MinecraftExtractUtils::filterMinecraftFile, _1, _2, dir);
    }

    static bool filterMinecraftFile(const char* filename, std::string& outName,
                                    std::string const& dir) {
        size_t nameLen = strlen(filename);
        if (nameLen >= 7 && memcmp(filename, "assets/", 7) == 0) {
            outName = dir + filename;
            return true;
        } else if (nameLen > 8 && memcmp(filename, "res/raw/", 8) == 0) {
            outName = dir + "assets/" + (filename + 8);
            return true;
        } else if (strcmp(filename, "res/drawable-xxxhdpi-v4/icon.png") == 0) {
            outName = dir + "assets/icon.png";
            return true;
        } else if (nameLen > 4 && memcmp(filename, "lib/", 4) == 0) {
            outName = dir + filename;
            return true;
        }
        return false;
    }


    static bool checkMinecraftLibFile(std::string const& dir) {
        if (!dir.empty() && dir[dir.length() - 1] != '/')
            return checkMinecraftLibFile(dir + "/");
    return 0
#ifdef __x86_64__
        || access((dir + "lib/x86_64/libminecraftpe.so").c_str(), R_OK) == 0
#endif
#if defined(__i386__) || defined(__x86_64__)
        || access((dir + "lib/x86/libminecraftpe.so").c_str(), R_OK) == 0
#endif
#ifdef __aarch64__
        || access((dir + "lib/arm64-v8a/libminecraftpe.so").c_str(), R_OK) == 0
#endif
#if defined(__arm__) || defined(__aarch64__)
        || access((dir + "lib/armeabi-v7a/libminecraftpe.so").c_str(), R_OK) == 0
#endif
        ;
    }

};