/**
 *  ./test-discover
 */

#include <iostream>
#include <sstream>
#include "nemr-5053-manufacturer-specific-data.h"
#include "ble-helper.h"

int main(int argc, char **argv) {
    BLEHelper b;

    b.startDiscovery();
    // b.waitDiscover("ff:ff:92:13:76:14");
    b.waitDiscover(1);
    for (auto &d : b.devices) {
        std::cout << macAddress2string(d.addr) << ' ' << d.metadata.toString() << ' ' << d.rssi << "dBm\n";
    }
    b.stopDiscovery(10);
    return 0;
}
