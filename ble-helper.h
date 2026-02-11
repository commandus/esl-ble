#ifndef BLE_HELPER_H
#define BLE_HELPER_H

#include <vector>
#include <map>
#include <chrono>
#include <mutex>

#include "nemr-5053-manufacturer-specific-data.h"

typedef std::chrono::time_point<std::chrono::system_clock> DISCOVERED_TIME;

enum CharacteristicIndex {
    CI_REQUEST = 0,
    CI_IMAGE = 1
};

class BLEDeviceImpl {
};

enum DeviceState {
    DS_IDLE = 0,
    DS_RUNNING,
    DS_SESSION_ON
};

class DiscoveredDevice {
public:
    /// Bluetooth address ff:ff:<dd>:<dd>:<dd>:<dd>
    uint64_t addr;
    /// last time discovered
    DISCOVERED_TIME dt;
    /// last discovered RSSI in dBm
    int16_t rssi;
    /// Discovered device spec
    NEMR5053ManufacturerSpecificData metadata;
    /// device name
    std::string name;
    /// session open or closed
    DeviceState deviceState;
    /// OS specific implementation
    BLEDeviceImpl *impl;

    DiscoveredDevice();
    DiscoveredDevice(uint64_t addr, int16_t rssi, const NEMR5053ManufacturerSpecificData &metadata, const std::string &name);
    ~DiscoveredDevice();
};

enum SendingStep {
    SS_STOPPED = 0,
    SS_GET_BLOCK_SIZE,
    SS_WRITE_SCREEN_SIZE,
    SS_START_TRANSFER,
    SS_TRANSFER_BLOCK,
    SS_DISCONNECT
};

class SendingState {
public:
    void *buffer;
    uint32_t size;
    uint16_t blockSize;
    uint32_t offset;
    uint8_t stepRetryCount;
    uint8_t stepTryCount;
    SendingState();
    SendingState(const SendingState&);
    ~SendingState();
};

class ReceivedData {
public:
    uint32_t size;
    void *data;
    ReceivedData();
    ReceivedData(void *data, uint32_t count);
    ReceivedData(const ReceivedData&);
    ~ReceivedData();
};

class BLEDiscoverer;

class OnDiscover {
public:
    BLEDiscoverer* discoverer;
    void *discoverExtra;
    OnDiscover();
    OnDiscover(BLEDiscoverer* discoverer);
    OnDiscover(BLEDiscoverer* discoverer, void *discoverExtra);
    virtual void discoverFirstTime(DiscoveredDevice &device) = 0;
    virtual void discoverNextTime(DiscoveredDevice &device) = 0;
};

class Image2sRgb;

class BLEDiscoverer {
public:
    bool discoveryOn;
    std::mutex mutexDiscoveryState;
    std::condition_variable cvDiscoveryState;

    std::vector<DiscoveredDevice> devices;
    std::map<uint64_t, ReceivedData> lastReceivedData;
    OnDiscover *onDiscover;

    BLEDiscoverer();
    BLEDiscoverer(OnDiscover *onDiscover);
    BLEDiscoverer(OnDiscover *onDiscover, void *discoverExtra);
    virtual int startDiscovery() = 0;
    virtual void stopDiscovery(int seconds = 10) = 0;
    virtual int open(DiscoveredDevice* device) = 0;
    virtual int close(DiscoveredDevice* device) = 0;
    virtual int read(const DiscoveredDevice *device, CharacteristicIndex characteristic, void *buffer, uint32_t size, int milliseconds = 2000) = 0;
    virtual int write(const DiscoveredDevice *device, CharacteristicIndex characteristic, void *buffer, uint32_t size) = 0;
    virtual int pair(const DiscoveredDevice *device) = 0;
    virtual int unpair(const DiscoveredDevice *device) = 0;

    const DiscoveredDevice& find(uint64_t addr);
    int waitDiscover(int deviceCount, int seconds = 20);
    bool waitDiscover(const char *addressString, int seconds = 20);
    // index versions
    int openI(int index);
    int closeI(int index);

    bool requestGetBlockSize(const DiscoveredDevice *device);
    bool requestSetScreenSize(const DiscoveredDevice *device, uint32_t value);
    bool requestStartTransfer(const DiscoveredDevice *device);
    bool requestCancelWrite(const DiscoveredDevice *device);
    bool requestWriteChunk(const DiscoveredDevice *device, uint32_t chunkNum, void* buffer, uint32_t ofs, uint8_t size);

    uint16_t getBlockSize(const DiscoveredDevice *device, int waitMs = 1000);
    bool setScreenSize(const DiscoveredDevice *device, uint32_t value, int waitMs = 1000);
    bool startTransfer(const DiscoveredDevice *device, int waitMs = 1000);
    bool cancelWrite(const DiscoveredDevice *device, int waitMs = 1000);
    int writeChunk(const DiscoveredDevice *device, uint32_t chunkNum, void* buffer, uint32_t ofs, uint8_t size, int waitMs = 1000);

    // index versions
    uint16_t getBlockSizeI(int deviceIndex, int waitMs = 1000);
    bool setScreenSizeI(int deviceIndex, uint32_t value, int waitMs = 1000);
    bool startTransferI(int deviceIndex, int waitMs = 1000);
    bool cancelWriteI(int deviceIndex, int waitMs = 1000);
    int writeChunkI(int deviceIndex, uint32_t chunkNum, void* buffer, uint32_t ofs, uint8_t size, int waitMs = 1000);

    int sendBuffer(const DiscoveredDevice *device, void *buffer, uint32_t size, int waitMs = 1000);
    int sendBufferI(int deviceIndex, void *buffer, uint32_t size, int waitMs = 1000);

    int writeSRgb(DiscoveredDevice *device, Image2sRgb *img);

};
#ifdef _MSC_VER
#include "ble-helper-win.h"
#endif

#endif
