include_directories(${CMAKE_CURRENT_LIST_DIR}
    #${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/audio
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/cdc
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/dfu
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/hid
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/hub
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/midi
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/msc
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/mtp
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/printer
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/vendor
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/video
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/wireless

    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/common
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/core
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/demo
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/port/rp2040
    ${CMAKE_CURRENT_LIST_DIR}../../
)


add_library(cherryusblib STATIC)
target_sources(cherryusblib PRIVATE ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/port/rp2040/usb_dc_rp2040.c)
target_sources(cherryusblib PRIVATE ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/audio/usbd_audio.c)
target_sources(cherryusblib PRIVATE ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/cdc/usbd_cdc.c)
target_sources(cherryusblib PRIVATE ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/dfu/usbd_dfu.c)
target_sources(cherryusblib PRIVATE ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/hid/usbd_hid.c)
target_sources(cherryusblib PRIVATE ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/msc/usbd_msc.c)
#target_sources(cherryusblib PRIVATE ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/mtp/usbd_mtp.c)
#target_sources(cherryusblib PRIVATE ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/printer/usbd_printer.c)
target_sources(cherryusblib PRIVATE ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/video/usbd_video.c)
target_sources(cherryusblib PRIVATE ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/wireless/usbd_rndis.c)
target_sources(cherryusblib PRIVATE ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/core/usbd_core.c)

add_subdirectory(device)