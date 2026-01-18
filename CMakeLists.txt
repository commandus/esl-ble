#
# mkdir build; cd build; cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\git\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows
#
cmake_minimum_required(VERSION 3.28)
set(VERSION_INFO 1.0.0)
project(esl-ble VERSION ${VERSION_INFO})

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # WinRT requires C++17 standard
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
    set(OS_SPECIFIC_LIBS wsock32 ws2_32 Userenv WindowsApp.lib)

    set(ESL_BLE_SRC
        nemr-5053-manufacturer-specific-data.cpp
        esl-device-known-types.cpp
        ble-helper.cpp
        ble-helper-win.cpp
        esl-string-helper-win.cpp
        srgb-pack.cpp
        image2srgb8.cpp
        png2srgb8.cpp
    )

else()
    set(CMAKE_CXX_STANDARD 11)
    set(ESL_BLE_SRC
        nemr-5053-manufacturer-specific-data.cpp
    )
endif()

set(ARGTABLE3_SRC third-party/argtable3/argtable3.c)

add_library(libesl-ble STATIC ${ESL_BLE_SRC})
target_include_directories(libesl-ble PRIVATE "." "third-party" ${VCPKG_INC})

add_executable(esl-ble esl-ble.cpp)
target_include_directories(esl-ble PRIVATE "." "third-party" ${VCPKG_INC})
target_link_libraries(esl-ble PRIVATE libesl-ble ${OS_SPECIFIC_LIBS} )

#
# Tests
#
add_subdirectory(tests)

# Print config
message("")
message(" Windows: must provide VCPKG options like -DCMAKE_TOOLCHAIN_FILE=C:\\git\\vcpkg\\")
message("   scripts\\buildsystems\\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows")
message("")
