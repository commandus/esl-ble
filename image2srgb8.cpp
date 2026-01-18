#include <filesystem>
#include <fstream>
#include "image2srgb8.h"

int32_t Image2sRgb::loadFile(
    const char* fileName
) {
    // Get size of file to know how much memory to allocate
    std::uintmax_t fileSize = std::filesystem::file_size(fileName);
    // Allocate buffer to hold file
    char *buf = new char[fileSize];
    if (!buf)
        return -1;
    // Read file
    std::ifstream fin(fileName, std::ios::binary);
    fin.read(buf, fileSize);
    if (!fin) {
        return -2;
    }
    fin.close();
    int r = load(buf, fileSize);
    free(buf);
    return r;
}
