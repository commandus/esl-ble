#include "png2srgb8.h"
#include "wpng/wpng_read.h"

int32_t Png2sRgb::load(
    void *srcPng,
    size_t srcPngSize
) {
    byte_buffer inBuffer = {(uint8_t *) srcPng, srcPngSize, srcPngSize, 0 };
    wpng_load_output output;
    memset(&output, 0, sizeof(wpng_load_output));
    uint32_t flags = WPNG_READ_FORCE_8BIT;
    wpng_load(&inBuffer, flags, &output);
    if (output.error)
        return -2;
    srgb = (SRgb8*) output.data;
    w = output.width;
    h = output.height;
    return (int32_t) output.size;
}
