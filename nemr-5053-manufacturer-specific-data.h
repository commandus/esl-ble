#ifndef ESL_BLE_NEMR_5053_MANUFACTURER_SPECIFIC_DATA_H
#define ESL_BLE_NEMR_5053_MANUFACTURER_SPECIFIC_DATA_H

#include "platform.h"
#include "esl-device-known-types.h"

enum PIXEL_SIZE {
    PS_212x104 = 0,
    PS_128x296,
    PS_400x300,
    PS_640x384,
    PS_960x640,
    PS_196x96,
    PS_640x480,
    PS_250x122
};

const char* pixelSizeString(enum PIXEL_SIZE);

enum MANUFACTURE_TYPE {
    MT_TFT = 0,
    MT_EPA = 1,
    MT_EPA_1 = 2,
    MT_UNKNOWN = 3
};

const char* manufactureTypeString(enum MANUFACTURE_TYPE);

enum COLOR_TYPE {
    CT_BW = 0,
    CT_BWR = 1,
    CT_BWY = 2,
    CT_UNKNOWN
};

const char* colorTypeString(enum COLOR_TYPE);

enum MIRROR_TYPE {
    MT_TWO = 0,
    MT_ONE = 1
};

const char* mirrorTypeString(enum MIRROR_TYPE);

PACK(
    struct NEMR5053_SCREEN_TYPE {
        union {
            uint8_t b;
            struct {
                uint8_t mirrorType : 1;
                uint8_t colorType : 2;
                uint8_t manufactureType: 2;
                uint8_t pixelSize: 3;
            };
        };
    };
);

PACK(
    struct NEMR5053_SCREEN_TYPE_2 {
        union {
            uint8_t b;
            struct {
                uint8_t rfu : 3;
                uint8_t deviceType : 5;
            };
        };
    };
);

PACK(
struct NEMR5053_MANUFACTURER_SPECIFIC_DATA {
    uint8_t magicNumber53;   // 0x53
    uint8_t magicNumber50;   // 0x50
    NEMR5053_SCREEN_TYPE typ;
    uint8_t volt10;   // voltage x 10
    uint8_t softwareVersion;   //
    uint8_t hardwareVersion;   // 1
    NEMR5053_SCREEN_TYPE_2 typ2;
    NEMR5053_MANUFACTURER_SPECIFIC_DATA& operator=(
        const NEMR5053_MANUFACTURER_SPECIFIC_DATA& other
    );
    std::string toString() const;
};
);

class NEMR5053ManufacturerSpecificData {
private:
    const ESLDeviceKnownType *findTyp2Size() const;
    std::string pixelSizeString(enum PIXEL_SIZE value) const;
public:
    struct NEMR5053_MANUFACTURER_SPECIFIC_DATA val;
    NEMR5053ManufacturerSpecificData();
    NEMR5053ManufacturerSpecificData(const void *buffer, size_t len);
    explicit NEMR5053ManufacturerSpecificData(const std::string &value);
    NEMR5053ManufacturerSpecificData(const NEMR5053ManufacturerSpecificData &value);
    bool valid() const;
    uint16_t width() const;
    uint16_t height() const;
    bool mirror() const;
    bool hasRed() const;
    bool hasYellow() const;
    uint8_t colorCount() const;
    uint8_t voltage10() const;
    uint8_t softwareVersion() const;
    uint8_t hardwareVersion() const;
    uint8_t type1() const;
    uint8_t type2() const;
    uint16_t type12() const;
    uint32_t screenSize() const;

    void setWidth(uint16_t width);
    void setHeight(uint16_t height);
    void setMirror(bool value);
    void setHasRed(bool value);
    void setHasYellow(bool value);
    void setVoltage10(uint8_t value);
    void setSoftwareVersion(uint8_t value);
    void setHardwareVersion(uint8_t value);
    std::string toString() const;
    std::string toJSONString() const;

    bool set(const void *buffer, size_t len);
};

#endif
