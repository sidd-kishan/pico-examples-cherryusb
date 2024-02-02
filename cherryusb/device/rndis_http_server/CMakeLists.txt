# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)

set(CONFIG_CHERRYUSB 1)
set(CONFIG_USB_DEVICE 1)
set(CONFIG_USB_DEVICE_RNDIS 1)
set(CONFIG_LWIP 1)
set(CONFIG_LWIP_HTTPSRV 1)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE flash_xip)
endif()

project(cherryusb_device_rndis_http_server)

include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/common ${CMAKE_CURRENT_SOURCE_DIR}/common/dhcp-server ${CMAKE_CURRENT_SOURCE_DIR}/common/dns-server ${PICO_EXAMPLES_PATH}/pico-sdk/lib/lwip/src/include ${PICO_EXAMPLES_PATH}/pico-sdk/lib/lwip/src/include/lwip/apps ${PICO_EXAMPLES_PATH}/pico-sdk/lib/tinyusb/hw/bsp/rp2040)
add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c ${CMAKE_CURRENT_SOURCE_DIR}/src/lwip.c ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c ${CMAKE_CURRENT_SOURCE_DIR}/src/wifi_tx.c ${CMAKE_CURRENT_SOURCE_DIR}/common/dhcp-server/dhserver.c ${CMAKE_CURRENT_SOURCE_DIR}/common/dns-server/dnserver.c ${CMAKE_CURRENT_SOURCE_DIR}/src/cdc_rndis_device.c)


target_link_libraries(${PROJECT_NAME} PRIVATE pico_stdlib cherryusblib hardware_resets hardware_irq pico_lwip pico_lwip_http pico_multicore  pico_cyw43_arch_none)
pico_add_extra_outputs(${PROJECT_NAME})