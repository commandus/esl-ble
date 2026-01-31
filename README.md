# esl-ble: C++ library with an example of writing images to electronic shelf labels (ESL) via BLE

## esl-ble

Example of the esl-ble utility for uploading an image to a single label. 

esl-ble uploads an image from a PNG file to the first detected label if the image width and
height match the size of the label's screen.

```shell
esl-ble <file.png>
``` 

The required parameter is the name of the image file.

Black and white colors are passed as is, without smoothing.

If the device supports red or yellow colors, they are sent to the device;
otherwise, yellow or red pixels will appear black.

## Library

Include the library depending on your build:

``` 
libesl-ble.lib
```

Include the header files:

```c++
#include "png2srgb8.h"
#include "ble-helper.h"
```

for importing PNG images from an array or file and sending images to the device.
Typically, the upload process consists of:
 
- converting images to sRGB format (4 bytes per pixel, first three - RGB, 
fourth byte - brightness)
- discovering devices in the background (asynchronously)
- obtaining the screen size of the detected shelf label
- converting the sRGB image taking into account the available colors of the device
and writing the image to it
- disabling device discovery

### Uploading a PNG File

```c++
Png2sRgb png;
int32_t sz = png.loadFile(fn);
// png.srgb - loaded sRGB image
std::cout << "width: " << png.w << " height: " << png.h << std::endl;
```

If sz is less than 0, the file and the image in it are corrupted and did not load.

The png.srgb member contains the image as an array of sRGB pixels in
the allocated memory.

### Uploading image files in other formats

If you want to load image files in other formats, you need to create a descendant
Image2sRgb class

```c++
class Jpeg2sRgb : public Image2sRgb {
public:
    int32_t load(void *srcJpeg, size_t srcPngSize) override;
};
```

with the load() method overridden.

The load() method retrieves a reference to the file content and the size in bytes.

The load() method should allocate memory on the heap for the class member srgb and
write the read image there in sRGB format.

The load() method returns the size of the sRGB image in bytes.

In case of failure, the load() method returns a negative number - the error code.

### ESL BLE detection

The BLEHelper object contains methods for detecting labels and writing images to them.
The startDiscovery() method starts the detection of BLE devices, which is performed
asynchronously.

```c++
    BLEHelper b;
    b.startDiscovery();
```

Methods

- waitDiscover(number)
- waitDiscover(string)

wait until the specified number of devices or a device with the given MAC address 
in the format "ff:ff:92:13:76:14" is detected. 

You need to specify all 6 bytes of the MAC address.
The last 4 numbers of the MAC address are written on the label, the first 
two numbers are always "ff:ff".

Both methods return the number of detected devices.

Both methods can time out. To change the default timeout of 20 seconds, pass
the timeout value in seconds as the second optional parameter.

In case of an error, both methods return a negative number - the error code.

The stopDiscovery() method stops asynchronous tag discovery.

```c++
    b.stopDiscovery(10);
```

The only optional parameter sets the timeout to wait for the method to complete
in seconds.

### Viewing Beacons

The devices member of the BLEHelper object contains a list of discovered beacons

```c++
    for (auto &d : b.devices) {
        std::cout << macAddress2string(d.addr) << ' ' << d.metadata.toString()
        << ' ' << d.rssi
        << "dBm. manufacturer specific data " 
        << d.metadata.val.toString() << "\n";
}
```

### Types of Tags

The ESLDeviceKnownType class refers to the eslDeviceKnownTypes1 static array with 
22 known label types.

The metadata.val member contains a manufacturer-specific byte array that the device
sends when a device is detected.

The first two bytes are just a magic number 0x5350, the same for all labels.

The last byte of this array, in its five most significant bits, contains the
device type.
In total, there can be 32 device types. So far, 22 types are known.

The third byte is described in the documentation as a flag field, which can
also be used to determine the device type.

The third and last bytes together define the device type, but since the 
documentation does not provide information about this, each device needs to be
checked individually.

### Callback method when an ESL is detected

If you need to process discovered labels as they are detected,
create a subclass of OnDiscover with the two methods overridden.

```c++
    class ExampleDiscover : public OnDiscover {
    public:
        void discoverFirstTime(DiscoveredDevice &device) override {
            std::cout << macAddress2string(device.addr) << ' ' << device.metadata.toString() << ' ' << device.rssi << "\n";
        }
    
        void discoverNextTime(DiscoveredDevice &device) override {
        }
    };
```

The discoverFirstTime() method is called when an ESL is detected for the first time,
and the discoverNextTime() method is called on all subsequent detections.

When creating a BLEHelper object, pass an instance of this class to the constructor
along with a pointer to the loaded image (or another object through which the image
will be provided).

```c++
    BLEHelper b(new ExampleDiscover(), &png);
```

## Building

Building is done using CMake for Visual Studio.
You can also use CLion with Visual Studio installed on your machine.

## Limitations

Currently, there is only a version for Windows 11 (Windows 10) with WinRT.

WinRT requires a compiler with C++17 support.

The device I have does not support compression and two-way mirroring, so this
is not implemented.

## MIT License

See the license in the LICENSE file.

## Tests

Tested on a label - 250x128 BWR EPA.

## How to make an bitmap image

For instance, you can use magick:

```shell
magick -size 250x128 xc:red -draw "fill white rectangle 0,64 249,128 fill black rectangle 0,0 125,64 font Arial font-size 36 fill white text 134,44 '$42.99' text  8,44 'SALE!' fill black text 2,94 'Green bananas' font-size 14 text 4, 120 'Organic product. Best before 11/12/26'" example250x128.png
```

## Links

- [Federico Poli. Gicisky Bluetooth ESL e-paper tag](https://github.com/fpoli/gicisky-tag)
- [zhihu.com. Протокол Bluetooth-тегов электронных этикеток полок Peake Smart](https://zhuanlan.zhihu.com/p/633113543)
- [Andrew  Graham. Picksmart/GiCiSky Electronic Shelf Label Image Uploader](https://shelflabels.andrewgraham.dev/)

## License

MIT License(see LICENSE file).

Copyright (C) 2026 Andrei Ivanov . All rights reserved.
