/**
 *  ./test-get-size
 */

#include <iostream>
#include <sstream>
#include "nemr-5053-manufacturer-specific-data.h"
#include "ble-helper.h"

int main(int argc, char **argv) {
    BLEHelper b;

    b.startDiscovery();
    // b.waitDiscover("ff:ff:92:13:76:14");
    auto devicesFound = b.waitDiscover(1);
    if (!devicesFound)
        return 0;
    for (auto &d : b.devices) {
        std::cout << macAddress2string(d.addr) << ' ' << d.metadata.toString() << ' ' << d.rssi << "dBm\n";
    }
    b.stopDiscovery(10);
    auto r = b.openI(0);
    if (r < 0) {
        std::cerr << "Error open device" << std::endl;
        return -1;
    }

    uint16_t sz = b.getBlockSizeI(0);
    std::cout << "Device size " << (int) sz << std::endl;
    if (sz == 0)
        return 0;

    r = b.closeI(0);
    if (r < 0) {
        std::cerr << "Error close device" << std::endl;
        return -1;
    }

    return 0;
}
