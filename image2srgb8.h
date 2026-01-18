#ifndef IMAGE2SRGB8_H
#define IMAGE2SRGB8_H

#include "srgb-pack.h"

class Image2sRgb {
public:
    SRgb8 *srgb;
    uint32_t w;
    uint32_t h;
    virtual int32_t load(void *src, size_t srcSize) = 0;
    int32_t loadFile(const char* fileName);
};

#endif
