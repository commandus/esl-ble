#include <sstream>
#include "esl-device-known-types.h"

static ESLDeviceKnownType eslDeviceKnownTypes1[] {
    { 0, 212, 104 },
    { 1, 296, 128 },
    { 2, 400, 300 },
    { 3, 640, 384 },
    { 4, 960, 640 },
    { 5, 250, 132 },
    { 6, 196, 96 },
    { 7, 640, 360 },
    { 8, 250, 128 }, //
    { 9, 800, 480 },
    { 10, 280, 480 },
    { 11, 1360, 480 },
    { 12, 168, 384 },
    { 13, 210, 480 },
    { 14, 1024, 576 },
    { 15, 384, 168 },
    { 16, 480, 210 },
    { 17, 240, 416 },
    { 18, 400, 600 },
    { 19, 1600, 1200 },
    { 20, 528, 768 },
    { 21, 200, 200 }, // last is 21
    { 82, 960, 680 },
    { 83, 792, 272 },
    { 84, 272, 792 }
};

/*
 * Out of range of 5 bit field
static ESLDeviceKnownType eslDeviceKnownTypes2[]{
    { 7,  640, 480 },
    { 8,  250, 122 },
    { 22, 200, 200 }
};
*/

/*
    P- pixel m- manu type(EPA,..) C- color M-mirror t- type
    MSD     PPPmmCCM ttttt
    0B41    00001011 01000(8)001   8  250x128 BWR EPA mirror-1
    0030    00000000 00110(6)000   1  296x128 BW
    0032    00000000 00110(6)010   1  296x128 BWR
    0049    00000000 01001(9)001   2  400x300 BW
    004B    00000000 01001(9)011   2  400x300 BWR
    40A0    01000000 10100(20)000  5  250x132 TFT
    4109    01000001 00001(1)001   8' 250x122 BW
    410B    01000001 00001(1)011   8' 250x122 BWR
    0129    00000001 00101(5)001   9  800x480 BW
    012B    00000001 00101(5)011   9  800x480 BWR
*/

const ESLDeviceKnownType *ESLDeviceKnownTypes::find(
    uint8_t typ2
) {
    if (typ2 <= 21)
        return &eslDeviceKnownTypes1[typ2];
    return nullptr;
}

std::string ESLDeviceKnownType::pixelSizeString() const {
    std::stringstream ss;
    ss << (int) w << 'x' << (int) h;
    return ss.str();
}
