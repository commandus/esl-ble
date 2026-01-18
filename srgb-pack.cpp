#include <cstring>
#include "srgb-pack.h"

inline void setBitOn(
    uint8_t *colorPlane,
    uint32_t heightInBytes,
    int x,
    int y
) {
    if (!colorPlane)
        return;
    int ofs = (x * heightInBytes) + (y / 8);
    *(colorPlane + ofs) |= 1 << (7 - (y % 8));
}

/*
 * ESL color planes order is B/W, red or yellow(if exists)
 * Each plane buffer growing from the left to the right column by column.
 * Each pixel in the column grouping in the bytes and growing from the top to the bottom.
 *
 *           B/W width
 *       +----------------+  red
 *       |^  ^            |-------+
 * height||\ |\           |       |
 *       || \|            |       |
 *       +----------------+       |
 *       1- white, 0- black   ----+
 *                         1- red, 0- none
 *       Second (red) color overlaps white and black colors
 */
/**
 * Convert sRGB 8-bit per color buffer to the buffer for ESL device
 * @param dst destination ESL device buffer
 * @param src sRGB buffer
 * @param width device screen width
 * @param height device screen height
 * @param hasRed red color
 * @param hasYellow yellow color
 * @param mirror false- double mirror, true- single mirror
 * @param compress compression on
 * @return
 */
int packSRgb8(
    void *dst,
    SRgb8 *src,
    uint32_t width,
    uint32_t height,
    bool hasRed,
    bool hasYellow,
    bool mirror,
    bool compress
)
{
    int planes = 1; // b/w
    if (hasRed)
        planes++;
    if (hasYellow)
        planes++;
    int heightInBytes = height / 8;
    if (height % 8)
        heightInBytes++;
    // clear all (set black/none)
    int planeBytes = heightInBytes * width;
    memset(dst, 0, planeBytes * planes);

    uint8_t *dstBW = (uint8_t *) dst;
    uint8_t *dstRed = hasRed ? ((uint8_t *) dst) + planeBytes : nullptr;
    uint8_t *dstYellow = hasYellow ? (hasRed ? ((uint8_t *) dst) + 2 * planeBytes : ((uint8_t *) dst) + planeBytes) : nullptr;

    SRgb8* color = src;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            bool red = (color->r > 150) && (color->r > (color->g + color->b));
            if (red)
                setBitOn(dstRed, heightInBytes, x, y);
            else {
                bool yellow = (color->r > 150) && (color->g > 150) && (color->b < 50);
                if (yellow)
                    setBitOn(dstYellow, heightInBytes, x, y);
                else {
                    bool black = (color->r + color->g + color->b) < 150;
                    if (!black)
                        setBitOn(dstBW, heightInBytes, x, y);;
                }
            }
            color++;
        }
    }
    return 0;
}
