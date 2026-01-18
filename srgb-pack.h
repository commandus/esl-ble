#ifndef SRGB_PACK_H
#define SRGB_PACK_H

#include <cstdint>

class SRgb8 {
public:
    uint8_t r, g, b, s;
};

int packSRgb8(void *dst, SRgb8* src, uint32_t width, uint32_t height, bool hasRed, bool hasYellow, bool mirror, bool compress);

#endif
