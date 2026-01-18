/**
 *  ./test-specific-data 53500b1c810141
 */

#include <iostream>
#include <sstream>
#include "nemr-5053-manufacturer-specific-data.h"
#include "ble-helper.h"

int main(int argc, char **argv) {
    BLEHelper b;

    uint8_t msd[7] { 0x53, 0x50, 0x0b, 0x1c, 0x81, 0x01, 0x41 };
    NEMR5053ManufacturerSpecificData h( &msd, sizeof(msd));
    if (argc > 1)
        h.set(argv[1], strlen(argv[1]));
    std::cout << h.toString() << std::endl;
    return 0;
}
