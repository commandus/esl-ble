/**
 *  ./test-png
 */

#include <iostream>
#include <sstream>
#include <iomanip>

#include "srgb-pack.h"
#include "png2srgb8.h"

static void printBWRY_RGB8(
    std::ostream &strm,
    SRgb8 *p,
    uint32_t width,
    uint32_t height
)
{
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            bool red = (p->r > 150) && (p->r > (p->g + p->b));
            char ch;
            if (red)
                ch = 'r';
            else {
                bool yellow = (p->r > 150) && (p->g > 150) && (p->b < 50);
                if (yellow)
                    ch = 'Y';
                else {
                    bool black = (p->r + p->g + p->b) < 150;
                    if (black)
                        ch = 'B';
                    else
                        ch = '_';
                }
            }
            strm << ch;
            p++;
        }
        strm << "\n";
    }
}

static void printSRGB8(
    std::ostream &strm,
    SRgb8 *p,
    uint32_t width,
    uint32_t height
)
{
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            strm << ' '
            << std::hex << std::setw(2) << std::setfill('0') << (int) p->s
            << std::hex << std::setw(2) << std::setfill('0') << (int) p->r
            << std::setw(2) << std::setfill('0') << (int) p->g
            << std::setw(2) << std::setfill('0') << (int) p->b << ' ';
            p++;
        }
        strm << "\n";
    }
}

int main(int argc, char **argv) {
    char *fn;
    if (argc > 1)
        fn = argv[1];
    else
        fn = "../../tests/250x128.png";
    Png2sRgb png;
    int32_t sz = png.loadFile(fn);
    if (sz < 0) {
        std::cerr << "Error parse file " << fn << std::endl;
        return sz;
    }
    std::cout << "width: " << png.w << " height: " << png.h << std::endl;
    printBWRY_RGB8(std::cout, png.srgb, png.w, png.h);
    // printSRGB8(std::cout, png.srgb, png.w, png.h);
    return 0;
}
