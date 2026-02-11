#include <iostream>
#include "ble-helper.h"
#include "image2srgb8.h"

SendingState::SendingState()
    : buffer(nullptr), size(0), blockSize(244), offset(0), stepRetryCount(3), stepTryCount(0)
{

}

SendingState::SendingState(
    const SendingState &state
)
    : buffer(state.buffer), size(state.size), blockSize(state.blockSize), offset(state.offset),
    stepRetryCount(state.stepRetryCount), stepTryCount(state.stepRetryCount)
{

}

SendingState::~SendingState()
{
    if (buffer)
        delete buffer;
}

ReceivedData::ReceivedData()
    : size(0), data(nullptr)
{
}

ReceivedData::ReceivedData(
    void *aData,
    uint32_t aSize
)
    : size(aSize), data(aData)
{
}

ReceivedData::ReceivedData(
    const ReceivedData &value
)
    : size(value.size), data(value.data)
{
}

ReceivedData::~ReceivedData() {
}

DiscoveredDevice::DiscoveredDevice()
    : addr(0), dt(std::chrono::system_clock::now()), rssi(0), deviceState(DS_IDLE),
    impl(nullptr)
{

}

DiscoveredDevice::DiscoveredDevice(
    uint64_t aAddr,
    int16_t aRssi,
    const NEMR5053ManufacturerSpecificData &aMetadata,
    const std::string &aName
)
    : addr(aAddr), dt(std::chrono::system_clock::now()), rssi(aRssi), metadata(aMetadata), name(aName), deviceState(DS_IDLE),
    impl(nullptr)
{

}

DiscoveredDevice::~DiscoveredDevice()
{
    if (impl) {
        delete impl;
        impl = nullptr;
    }
}

BLEDiscoverer::BLEDiscoverer()
    : discoveryOn(false), onDiscover(nullptr)
{

}

BLEDiscoverer::BLEDiscoverer(
    OnDiscover *aOnDiscover
)
    : discoveryOn(false), onDiscover(aOnDiscover)
{
    if (aOnDiscover) {
        aOnDiscover->discoverer = this;
        aOnDiscover->discoverExtra = nullptr;
    }
}

BLEDiscoverer::BLEDiscoverer(
    OnDiscover *aOnDiscover,
    void *aDiscoverExtra
)
    : discoveryOn(false), onDiscover(aOnDiscover)
{
    if (aOnDiscover) {
        aOnDiscover->discoverer = this;
        aOnDiscover->discoverExtra = aDiscoverExtra;
    }
}

bool BLEDiscoverer::waitDiscover(
    const char *addressString,
    int seconds
) {
    // wait
    std::unique_lock<std::mutex> lock(mutexDiscoveryState);
    uint64_t a = string2macAddress(addressString);
    cvDiscoveryState.wait_for(lock, std::chrono::seconds(seconds), [this, a] {
        bool found = false;
        for (auto &d : devices) {
            if (d.addr == a) {
                found = true;
                break;
            }
        }
        return found;
    });
    return false;
}

int BLEDiscoverer::waitDiscover(
    int deviceCount,
    int seconds
) {
    // wait
    std::unique_lock<std::mutex> lock(mutexDiscoveryState);
    cvDiscoveryState.wait_for(lock, std::chrono::seconds(seconds), [this, deviceCount] {
        return devices.size() >= deviceCount;
    });
    return (int) devices.size();
}

bool BLEDiscoverer::requestGetBlockSize(
    const DiscoveredDevice *device
) {
    uint8_t buffer[1] { 1 };
    return write(device, CI_REQUEST, buffer, 1) == 1;
}

bool BLEDiscoverer::requestSetScreenSize(
    const DiscoveredDevice *device,
    uint32_t value
) {
    uint8_t buffer[8] {2, 0, 0, 0, 0, 0, 0, 0};
#if IS_BIG_ENDIAN
    value = SWAP_BYTES_4(value);
#endif
    memmove(buffer + 1, &value, 4);
    return write(device, CI_REQUEST, buffer, sizeof(buffer)) == sizeof(buffer);
}

bool BLEDiscoverer::requestStartTransfer(
    const DiscoveredDevice *device
) {
    uint8_t buffer[1] { 3 };
    return write(device, CI_REQUEST, buffer, 1) == 1;
}

bool BLEDiscoverer::requestCancelWrite(
    const DiscoveredDevice *device
) {
    uint8_t buffer[1] { 4 };
    return write(device, CI_REQUEST, buffer, 1) == 1;
}

bool BLEDiscoverer::requestWriteChunk(
    const DiscoveredDevice *device,
    uint32_t chunkNum,
    void* buf,
    uint32_t ofs,
    uint8_t size
) {
    void* buffer = calloc(size + 4, 1);
    if (!buffer)
        return false;   // out of memory
#if IS_BIG_ENDIAN
    uint32_t wchunkNum = SWAP_BYTES_4(ofs);
#else
    uint32_t wchunkNum = chunkNum;
#endif
    memmove(buffer, &wchunkNum, 4);
    auto *p = (uint8_t *) buf;
    memmove((uint8_t *) buffer + 4, p + ofs, size);
    int c = write(device, CI_IMAGE, buffer, size + 4);
    free(buffer);
    return c == size + 4;
}

uint16_t BLEDiscoverer::getBlockSize(
    const DiscoveredDevice *device,
    int waitMs
)
{
    if (!requestGetBlockSize(device))
        return -1;
    uint8_t buffer[3];
    // read response
    auto r = read(device, CI_REQUEST, buffer, 3, waitMs);
    if (r < 3)
        return 0;
    if (buffer[0] != 1)
        return 0;
#if IS_BIG_ENDIAN
    return SWAP_BYTES_2(*((uint16_t*) (buffer + 1)));
#else
    return *((uint16_t*) (buffer + 1));
#endif
}

bool BLEDiscoverer::setScreenSize(
    const DiscoveredDevice *device,
    uint32_t value,
    int waitMs
)
{
    if (!requestSetScreenSize(device, value))
        return false;
    uint8_t buffer[2];
    // read response
    auto r = read(device, CI_REQUEST, buffer, 2, waitMs);
    if (r < 2)
        return false;
    if (buffer[0] != 2)
        return false;
    return buffer[1] == 0;
}

bool BLEDiscoverer::startTransfer(
    const DiscoveredDevice *device,
    int waitMs
)
{
    if (!requestStartTransfer(device))
        return false;
    uint8_t buffer[6];
    // read response
    auto r = read(device, CI_REQUEST, buffer, 6, waitMs);
    if (r < 2)
        return false;
    if (buffer[0] != 5)
        return false;
    uint8_t status = buffer[1];
    if (r >= 6) {
        uint32_t ofs;
        memmove(&ofs, buffer + 2, 4);
#if IS_BIG_ENDIAN
        ofs = SWAP_BYTES_4(ofs);
#endif
    }
    return status == 0;
}

bool BLEDiscoverer::cancelWrite(
    const DiscoveredDevice *device,
    int waitMs
)
{
    if (!requestCancelWrite(device))
        return false;
    uint8_t buffer[2];
    // read response
    auto r = read(device, CI_REQUEST, buffer, 2, waitMs);
    if (r != 2)
        return false;
    if (buffer[0] != 4)
        return false;
    return buffer[1] == 0;
}

int BLEDiscoverer::writeChunk(
    const DiscoveredDevice *device,
    uint32_t chunkIdx,
    void *buf,
    uint32_t ofs,
    uint8_t size,
    int waitMs
) {
    if (!requestWriteChunk(device, chunkIdx, buf, ofs, size))
        return -1;
    uint8_t buffer[6];
    // read response
    auto r = read(device, CI_REQUEST, buffer, 6, 99000 + waitMs);
    if (r < 2)
        return -1;
    if (buffer[0] != 5)
        return -1;
    if (r < 6)
        return 0;
    if (buffer[1] == 8)
        return -8; // all done
    else {
        uint32_t nextChunk;
        memmove(&nextChunk, buffer + 2, sizeof(uint32_t ));
#if IS_BIG_ENDIAN
        nextChunk = SWAP_BYTES_4(nextChunk);
#endif
        return (int) nextChunk;
    }
}

int BLEDiscoverer::openI(
    int index
) {
    if (index >= devices.size())
        return -1;
    return open(&devices[index]);
}

int BLEDiscoverer::closeI(
    int index
) {
    if (index >= devices.size())
        return -1;
    return close(&devices[index]);
}

uint16_t BLEDiscoverer::getBlockSizeI(
    int deviceIndex,
    int waitMs
) {
    if (deviceIndex >= devices.size())
        return 0;
    return getBlockSize(&devices[deviceIndex], waitMs);
}

bool BLEDiscoverer::setScreenSizeI(
    int deviceIndex,
    uint32_t value,
    int waitMs
) {
    if (deviceIndex >= devices.size())
        return false;
    return setScreenSize(&devices[deviceIndex], value, waitMs);
}

bool BLEDiscoverer::startTransferI(
    int deviceIndex,
    int waitMs
) {
    if (deviceIndex >= devices.size())
        return false;
    return startTransfer(&devices[deviceIndex], waitMs);
}

bool BLEDiscoverer::cancelWriteI(
    int deviceIndex,
    int waitMs
) {
    if (deviceIndex >= devices.size())
        return false;
    return cancelWrite(&devices[deviceIndex], waitMs);
}

int BLEDiscoverer::writeChunkI(
    int deviceIndex,
    uint32_t chunkIdx,
    void *buffer,
    uint32_t ofs,
    uint8_t size,
    int waitMs
) {
    if (deviceIndex >= devices.size())
        return -1;
    return writeChunk(&devices[deviceIndex], chunkIdx, buffer, ofs, size, waitMs);
}

/**
 *
 * @param device
 * @param buffer
 * @param size
 * @param waitMs
 * @return 0- success
 */
int BLEDiscoverer::sendBuffer(
    const DiscoveredDevice *device,
    void *buffer,
    uint32_t size,
    int waitMs
) {
    uint16_t blockSize = 0;
    int stepTryCount = 3;
    for (int i = 0; i < stepTryCount; i++) {
        blockSize = getBlockSize(device, waitMs);
        if (blockSize > 0)
            break;
    }
    if (blockSize == 0)
        return -1;

    bool r;
    for (int i = 0; i < stepTryCount; i++) {
        r = setScreenSize(device, size, waitMs);
        if (r)
            break;
    }
    if (!r)
        return -2;

    for (int i = 0; i < stepTryCount; i++) {
        r = startTransfer(device, waitMs);
        if (r)
            break;
    }
    if (!r)
        return -3;

    uint32_t chunkSize = blockSize - 4;
    int chunksCount = (int) (size / chunkSize);
    if (size % chunkSize)
        chunksCount++;
    int chunkNum = 0;
    while (chunkNum >= 0) {
        auto chunkOfs = chunkNum * chunkSize;
        auto nextOfs = (chunkNum + 1) * chunkSize;
        if (nextOfs > size)
            nextOfs = size;
        auto sz = nextOfs - chunkOfs;
        for (int i = 0; i < stepTryCount; i++) {
            chunkNum = writeChunk(device, chunkNum, buffer, chunkOfs, sz, waitMs);
            if (chunkNum >= 0 || chunkNum == -8)
                break;
        }
        if (!r)
            return -4;
    }
    return 0;
}

int BLEDiscoverer::sendBufferI(
    int deviceIndex,
    void *buffer,
    uint32_t size,
    int waitMs
) {
    if (deviceIndex >= devices.size())
        return -1;
    return sendBuffer(&devices[deviceIndex], buffer, size, waitMs);
}

const DiscoveredDevice notFoundDevice;

const DiscoveredDevice& BLEDiscoverer::find(
    uint64_t addr
) {
    auto it = std::find_if(devices.begin(), devices.end(), [addr](const DiscoveredDevice &dev) {
        return dev.addr == addr;
    });
    if (it == devices.end())
        return notFoundDevice;
    return *it;
}

int BLEDiscoverer::writeSRgb(
    DiscoveredDevice *device,
    Image2sRgb *img
) {
    if (device->metadata.width() != img->w || device->metadata.height() != img->h)
        return -1;
    int r = open(device);
    if (r < 0) {
        // std::cerr << "Error open device" << std::endl;
        return r;
    }

    uint32_t imgBytes = device->metadata.screenSize();
    uint8_t *imgBuffer = (uint8_t *) malloc(imgBytes);
    if (imgBuffer) {
        packSRgb8(imgBuffer, img->srgb, img->w, img->h, device->metadata.hasRed(), device->metadata.hasYellow(),
                  device->metadata.mirror(), false);
        r = sendBuffer(device, imgBuffer, imgBytes);
        if (r) {
            // std::cerr << "Error send screen to device" << r << std::endl;
        }
        free(imgBuffer);
    } else {
        // std::cerr << "Insufficient memory" << std::endl;
        r = -2;
    }
    r = close(device);
    if (r < 0) {
        // std::cerr << "Error close device" << std::endl;
    }
    return r;
}

OnDiscover::OnDiscover()
    : discoverer(nullptr)
{

}

OnDiscover::OnDiscover(
    BLEDiscoverer *aDiscoverer
)
    : discoverer(aDiscoverer)
{

}
