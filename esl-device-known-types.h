#ifndef ESL_DEVICE_KNOWN_TYPES_H
#define ESL_DEVICE_KNOWN_TYPES_H

#include <cstdint>
#include <string>

class ESLDeviceKnownType {
public:
    uint8_t typ2;
    uint16_t w;
    uint16_t h;
    std::string pixelSizeString() const;
};

class ESLDeviceKnownTypes {
public:
    static const ESLDeviceKnownType* find(uint8_t typ2);
};

#endif
