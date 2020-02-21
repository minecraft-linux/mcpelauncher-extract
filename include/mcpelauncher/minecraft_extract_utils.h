#pragma once

#include <string>
#include <functional>
#include <cstring>
#include <unistd.h>

class MinecraftExtractUtils {

public:
    static std::function<bool (const char* filename, std::string& outName)>
    filterMinecraftFiles(std::string const& dir, std::string const& arch = "x86") {
        if (!dir.empty() && dir[dir.length() - 1] != '/')
            return filterMinecraftFiles(dir + "/", arch);
        using namespace std::placeholders;
        return std::bind(&MinecraftExtractUtils::filterMinecraftFile, _1, _2, dir, arch);
    }

    static bool filterMinecraftFile(const char* filename, std::string& outName,
                                    std::string const& dir, std::string const& arch) {
        size_t nameLen = strlen(filename);
        if (nameLen >= 7 && memcmp(filename, "assets/", 7) == 0) {
            outName = dir + filename;
            return true;
        } else if (strcmp(filename, "res/raw/xboxservices.config") == 0) {
            outName = dir + "assets/xboxservices.config";
            return true;
        } else if (strcmp(filename, "res/drawable-xxxhdpi-v4/icon.png") == 0) {
            outName = dir + "assets/icon.png";
            return true;
        } else if (nameLen == 4 + arch.length() + 18 &&
                memcmp(filename, "lib/", 4) == 0 &&
                memcmp(&filename[4], arch.data(), arch.length()) == 0 &&
                memcmp(&filename[4 + arch.length()], "/libminecraftpe.so", 18) == 0) {
            outName = dir + "libs/libminecraftpe.so";
            return true;
        } else if (nameLen == 4 + arch.length() + 20 &&
                memcmp(filename, "lib/", 4) == 0 &&
                memcmp(&filename[4], arch.data(), arch.length()) == 0 &&
                memcmp(&filename[4 + arch.length()], "/libgnustl_shared.so", 20) == 0) {
            outName = dir + "libs/libgnustl_shared.so";
            return true;
        }
        return false;
    }


    static bool checkMinecraftLibFile(std::string const& dir) {
        if (!dir.empty() && dir[dir.length() - 1] != '/')
            return checkMinecraftLibFile(dir + "/");
        return access((dir + "libs/libminecraftpe.so").c_str(), R_OK) == 0;
    }

};