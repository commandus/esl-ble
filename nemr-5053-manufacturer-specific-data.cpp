#include <sstream>
#include "nemr-5053-manufacturer-specific-data.h"
#include "esl-string-helper-win.h"

static uint16_t PIXEL_WIDTH_0_7[8] {212, 128, 400, 640, 960, 196, 640, 250 };
static uint16_t PIXEL_HEIGHT_0_7[8] {104, 296, 300, 384, 640, 96, 480, 122 };

NEMR5053_MANUFACTURER_SPECIFIC_DATA& NEMR5053_MANUFACTURER_SPECIFIC_DATA::operator=(
    const NEMR5053_MANUFACTURER_SPECIFIC_DATA& other
) {
    memmove(&magicNumber50, &other.magicNumber50, sizeof(NEMR5053_MANUFACTURER_SPECIFIC_DATA));
    return *this;
}

std::string NEMR5053_MANUFACTURER_SPECIFIC_DATA::toString() const
{
    return hex((void*) &magicNumber53, 7);
}

NEMR5053ManufacturerSpecificData::NEMR5053ManufacturerSpecificData()
    : val({ 0, 0, 0, 30, 0, 1 })
{

}

NEMR5053ManufacturerSpecificData::NEMR5053ManufacturerSpecificData(
    const void *buffer, size_t len
)
    : NEMR5053ManufacturerSpecificData()
{
    if (len >= sizeof(val)) {
        if (len >= 2 * sizeof(val)) {
            std::string s((char *) buffer, len);
            if (isHex(s)) {
                s = hex2string(s);
                memmove(&val, s.c_str(), s.size());
                return;
            }
        }
        memmove(&val, buffer, sizeof(val));
    }
}

NEMR5053ManufacturerSpecificData::NEMR5053ManufacturerSpecificData(
    const std::string &value
)
    : NEMR5053ManufacturerSpecificData(value.c_str(), value.size())
{

}

bool NEMR5053ManufacturerSpecificData::valid() const
{
    return val.magicNumber50 == 0x50 && val.magicNumber53 == 0x53;
}

uint16_t NEMR5053ManufacturerSpecificData::width() const
{
    auto kts = findTyp2Size();
    if (kts)
        return kts->w;
    return PIXEL_WIDTH_0_7[val.typ.pixelSize];
}

uint16_t NEMR5053ManufacturerSpecificData::height() const
{
    auto kts = findTyp2Size();
    if (kts)
        return kts->h;
    return PIXEL_HEIGHT_0_7[val.typ.pixelSize];
}

bool NEMR5053ManufacturerSpecificData::mirror() const
{
    return val.typ.mirrorType == MT_ONE;
}

bool NEMR5053ManufacturerSpecificData::hasRed() const
{
    return val.typ.colorType == CT_BWR;
}

bool NEMR5053ManufacturerSpecificData::hasYellow() const
{
    return val.typ.colorType == CT_BWY;
}

uint8_t NEMR5053ManufacturerSpecificData::colorCount() const {
    return val.typ.colorType > CT_BW ? 2 : 1;
}

uint32_t NEMR5053ManufacturerSpecificData::screenSize() const {
    auto lineBytes = height() / 8;
    if (height() % 8)
        lineBytes++;
    return lineBytes * width() * colorCount();
}

uint8_t NEMR5053ManufacturerSpecificData::voltage10() const
{
    return val.volt10;;
}

uint8_t NEMR5053ManufacturerSpecificData::softwareVersion() const
{
    return val.softwareVersion;
}

uint8_t NEMR5053ManufacturerSpecificData::hardwareVersion() const
{
    return val.hardwareVersion;
}

void NEMR5053ManufacturerSpecificData::setWidth(
    uint16_t width
) {
    auto kts = findTyp2Size();
    if (kts)
        return;
    for (int i = 0; i < 4; i++) {
        if (PIXEL_WIDTH_0_7[i] == width) {
            val.typ.pixelSize = i;
            return;
        }
    }
}

void NEMR5053ManufacturerSpecificData::setHeight(
    uint16_t height
) {
    auto kts = findTyp2Size();
    if (kts)
        return;
    for (int i = 0; i < 4; i++) {
        if (PIXEL_HEIGHT_0_7[i] == height) {
            val.typ.pixelSize = i;
            return;
        }
    }
}

void NEMR5053ManufacturerSpecificData::setMirror(
    bool value
) {
    val.typ.mirrorType = value ? MT_ONE : MT_TWO;
}

void NEMR5053ManufacturerSpecificData::setHasRed(
    bool value
) {
    val.typ.colorType = value ? CT_BWR : CT_BW;
}

void NEMR5053ManufacturerSpecificData::setHasYellow(
    bool value
) {
    val.typ.colorType = value ? CT_BWY : CT_BW;
}

void NEMR5053ManufacturerSpecificData::setVoltage10(
    uint8_t value
) {
    val.volt10 = value;
}

void NEMR5053ManufacturerSpecificData::setSoftwareVersion(
    uint8_t value
) {
    val.softwareVersion = value;
}

void NEMR5053ManufacturerSpecificData::setHardwareVersion(
        uint8_t value
) {
    val.hardwareVersion = value;
}

std::string NEMR5053ManufacturerSpecificData::toString() const {
    std::stringstream ss;
    ss << colorTypeString((COLOR_TYPE) val.typ.colorType)
        << pixelSizeString((PIXEL_SIZE) val.typ.pixelSize)
        << mirrorTypeString((MIRROR_TYPE) val.typ.mirrorType)
        << ' ' << manufactureTypeString((MANUFACTURE_TYPE) val.typ.manufactureType)
        << ' ' << (int) (val.volt10 / 10) << '.' << (int) (val.volt10 % 10) << 'V';
    return ss.str();
}

std::string NEMR5053ManufacturerSpecificData::toJSONString() const {
    std::stringstream ss;
    ss << R"({"color": ")" << colorTypeString((COLOR_TYPE) val.typ.colorType)
        << R"(", "width": )" << width()
        << ", \"height\": " << height()
        << R"(, "mirror": ")" << mirrorTypeString((MIRROR_TYPE) val.typ.mirrorType)
        << R"(", "voltage10": )" << (int) val.volt10 << '}';
    return ss.str();
}

bool NEMR5053ManufacturerSpecificData::set(
    const void *buffer,
    size_t len
) {
    if (len >= sizeof(NEMR5053_MANUFACTURER_SPECIFIC_DATA)) {
        if (len >= 2 * sizeof(NEMR5053_MANUFACTURER_SPECIFIC_DATA)) {
            std::string s((char *) buffer, len);
            if (isHex(s)) {
                s = hex2string(s);
                memmove(&val, s.c_str(), s.size());
                return valid();
            }
        }
        memmove(&val, buffer, sizeof(NEMR5053_MANUFACTURER_SPECIFIC_DATA));
        return valid();
    }
    return false;
}

NEMR5053ManufacturerSpecificData::NEMR5053ManufacturerSpecificData(
    const NEMR5053ManufacturerSpecificData &value
)
    : val(value.val)
{

}

uint8_t NEMR5053ManufacturerSpecificData::type1() const {
    return val.typ.b;
}

uint8_t NEMR5053ManufacturerSpecificData::type2() const {
    return val.typ2.b;
}

uint16_t NEMR5053ManufacturerSpecificData::type12() const {
    return (val.typ.b) | (val.typ2.b << 8);
}

const ESLDeviceKnownType *NEMR5053ManufacturerSpecificData::findTyp2Size() const
{
    return ESLDeviceKnownTypes::find(val.typ2.deviceType);
}

std::string NEMR5053ManufacturerSpecificData::pixelSizeString(
    enum PIXEL_SIZE value
) const {
    auto kts = findTyp2Size();
    if (kts)
        return kts->pixelSizeString();
    std::stringstream ss;
    ss << PIXEL_WIDTH_0_7[(int)value] << 'x' << PIXEL_HEIGHT_0_7[(int)value];
    return ss.str();
}

static const char* MANUFACTURE_TYPE_STR[] {
    "TFT",
    "EPA",
    "EPA-1",
    "unknown"
};

const char *manufactureTypeString(
    enum MANUFACTURE_TYPE value
) {
    return MANUFACTURE_TYPE_STR[(int) value];
}

static const char* COLOR_TYPE_STR[] {
    "BW",
    "BWR",
    "BWY",
    "unknown"
};

const char *colorTypeString(
    enum COLOR_TYPE value
) {
    return COLOR_TYPE_STR[(int) value];
}

static const char* MIRROR_TYPE_STR[] {
    "M-M",
    "M"
};

const char *mirrorTypeString(
    enum MIRROR_TYPE value
) {
    return MIRROR_TYPE_STR[(int) value];
}
