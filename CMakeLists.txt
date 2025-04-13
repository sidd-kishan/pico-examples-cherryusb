cmake_minimum_required(VERSION 3.16)

set(PICO_BOARD pico_w)
include(pico-sdk/pico_sdk_init.cmake)

#include(pico_extras_import_optional.cmake)

project(pico_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

set(PICO_LWIP_CORE_CODE ${PICO_EXAMPLES_PATH}/pico-sdk/lib/lwip/src/core/)
# Initialize the SDK
pico_sdk_init()

include(example_auto_set_url.cmake)
#set(HW_RESETS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk/src/rp2_common/hardware_resets/include ${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk/src/rp2040/hardware_structs/include ${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk/src/rp2_common/hardware_base/include ${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk/src/rp2040/hardware_regs/include ${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk/src/host/hardware_irq/include ${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk/src/common/pico_stdlib_headers/include ${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk/src/rp2_common/pico_stdio/include ${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk/src/common/pico_time/include ${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk/src/rp2_common/hardware_timer/include ${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk/src/rp2_common/hardware_gpio/include ${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk/src/rp2_common/hardware_uart/include)
set(HW_RESETS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk/src/rp2_common/hardware_resets/include ${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk/src/rp2040/hardware_structs/include ${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk/src/rp2_common/hardware_base/include ${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk/src/rp2040/hardware_regs/include ${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk/src/host/hardware_irq/include ${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk/src/rp2040/pico_platform/include ${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk/src/rp2_common/pico_platform_compiler/include ${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk/src/common/pico_base_headers/include)
set(PICO_H_REQUIRED ${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk/src/boards/include/boards/)
set(LWIP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk/lib/lwip)
set (LWIP_INCLUDE_DIRS
    "${LWIP_DIR}/src/include"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Add blink example
#add_subdirectory(blink)

# Add hello world example
#add_subdirectory(hello_world)

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

# Hardware-specific examples in subdirectories:
#add_subdirectory(adc)
#add_subdirectory(clocks)
#add_subdirectory(cmake)
#add_subdirectory(divider)
#add_subdirectory(dma)
#add_subdirectory(flash)
#add_subdirectory(gpio)
#add_subdirectory(i2c)
#add_subdirectory(interp)
#add_subdirectory(multicore)
#add_subdirectory(picoboard)
#add_subdirectory(pico_w)
#add_subdirectory(pio)
#add_subdirectory(pwm)
#add_subdirectory(reset)
#add_subdirectory(rtc)
#add_subdirectory(spi)
#add_subdirectory(system)
#add_subdirectory(timer)
#add_subdirectory(uart)
#add_subdirectory(usb)
#add_subdirectory(watchdog)
add_subdirectory(cherryusb)
