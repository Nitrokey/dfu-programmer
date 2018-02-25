cmake_minimum_required(VERSION 3.4)
project(dfu_programmer C)

set(CMAKE_C_STANDARD 11)

set(SRC
        src/arguments.c
        src/arguments.h
        src/atmel.c
        src/atmel.h
        src/commands.c
        src/commands.h
        src/dfu-bool.h
        src/dfu-device.h
        src/dfu.c
        src/dfu.h
        src/intel_hex.c
        src/intel_hex.h
        src/stm32.c
        src/stm32.h
        src/util.c
        src/util.h)

# define targets
add_executable(dfu_programmer
        src/main.c
        )
add_executable(libtest
        libtest.c
        )

add_library(dfup STATIC ${SRC}
        src/lib_implementation.c src/lib_implementation.h
        )

#add_definitions(-DHAVE_CONFIG_H -MD -MP -MF -c -O2)

# setup linking
IF(APPLE)
    find_library(corefoundation_lib CoreFoundation)
    find_library(iokit_lib IOKit)

    set(frameworks
        ${iokit_lib}
        ${corefoundation_lib})
ENDIF()

IF(APPLE)
    # only for compilation time, to link libusb statically
    # brew.sh install libusb
    target_link_libraries(dfup ${frameworks} /usr/local/opt/libusb/lib/libusb-1.0.a)
ELSE()
    target_link_libraries(dfup usb-1.0)
ENDIF()

target_link_libraries(libtest dfup)
target_link_libraries(dfu_programmer dfup)

# additional settings
target_compile_definitions(dfu_programmer PUBLIC HAVE_CONFIG_H)
target_compile_definitions(dfup PUBLIC HAVE_CONFIG_H)
target_compile_definitions(libtest PUBLIC HAVE_CONFIG_H)

# include bundled libusb headers
include_directories(3rdparty/libusb-v1.0.22-0-g0034b2a/libusb)
