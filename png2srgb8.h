#ifndef PNG2SRGB8_H
#define PNG2SRGB8_H

#include "image2srgb8.h"

class Png2sRgb : public Image2sRgb {
public:
    int32_t load(void *srcPng, size_t srcPngSize) override;
};

#endif
