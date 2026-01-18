cmake_minimum_required(VERSION 3.28)

project("esl-ble-test")

# WinRT requires C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(TEST_INCS "." ".." "../third-party" ${VCPKG_INC})
set(TEST_LIBS libesl-ble ${OS_SPECIFIC_LIBS})

add_executable(test-specific-data test-specific-data.cpp)
target_include_directories(test-specific-data PRIVATE ${TEST_INCS})
target_link_libraries(test-specific-data PRIVATE ${TEST_LIBS})
add_test(NAME test-specific-data COMMAND "test-specific-data")

add_executable(test-discover test-discover.cpp)
target_include_directories(test-discover PRIVATE ${TEST_INCS})
target_link_libraries(test-discover PRIVATE ${TEST_LIBS})
add_test(NAME test-discover COMMAND "test-discover")

add_executable(test-get-size test-get-size.cpp)
target_include_directories(test-get-size PRIVATE ${TEST_INCS})
target_link_libraries(test-get-size PRIVATE ${TEST_LIBS})
add_test(NAME test-get-size COMMAND "test-get-size")

add_executable(test-send-figure test-send-figure.cpp)
target_include_directories(test-send-figure PRIVATE ${TEST_INCS})
target_link_libraries(test-send-figure PRIVATE ${TEST_LIBS})
add_test(NAME test-send-figure COMMAND "test-send-figure")

add_executable(test-png test-png.cpp)
target_include_directories(test-png PRIVATE ${TEST_INCS})
target_link_libraries(test-png PRIVATE ${TEST_LIBS})
add_test(NAME test-png COMMAND "test-png")
