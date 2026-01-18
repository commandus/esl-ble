/**
 *  ./test-send-figure
 */

#include <iostream>
#include <sstream>
#include "nemr-5053-manufacturer-specific-data.h"
#include "ble-helper.h"
#include "png2srgb8.h"

/*
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
void drawWhiteBlackRedRectangles(
    void *buffer,
    uint32_t size,
    uint16_t width,
    uint16_t height,
    uint8_t colorCount
) {
    // black & none
    memset(buffer, 0, size);
    // white
    memset(buffer, 0xff, size / 4);
    if (colorCount > 1) {
        // red
        auto *p = (uint8_t *) buffer;
        memset(p + (size / 2) + size / 8, 0xff, size / 4);
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

    BLEHelper b;
    b.startDiscovery();
    // b.waitDiscover("ff:ff:92:13:76:14");
    auto devicesFound = b.waitDiscover(1);
    if (!devicesFound)
        return 0;
    for (auto &d : b.devices) {
        std::cout << macAddress2string(d.addr) << ' ' << d.metadata.toString() << ' ' << d.rssi << "dBm. manufacturer specific data  " << d.metadata.val.toString() << "\n";
    }
    b.stopDiscovery(10);
    int r = b.writeSRgb(&b.devices[0], &png);
    if (r < 0) {
        std::cerr << "Error write device" << std::endl;
    }
    return r;
}
