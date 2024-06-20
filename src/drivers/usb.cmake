include_guard()

message(STATUS "Adding USB driver for class: ${CONFIG_USB_CLASS}")

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/usb
                                            ${CMAKE_CURRENT_LIST_DIR}/usb/udc)

target_sources(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/usb/udc/udc.c)

if(${MCU_FAMILY} STREQUAL "XMEGA")

  include(nvm)

  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_CURRENT_LIST_DIR}/usb/xmega)

  target_sources(${TARGET}
                 PUBLIC ${CMAKE_CURRENT_LIST_DIR}/usb/xmega/usb_device.c)

else()
  message(FATAL_ERROR "Unsupported MCU family: ${MCU_FAMILY}")
endif()

if(${CONFIG_USB_CLASS} STREQUAL "CDC")

  target_include_directories(
    ${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/usb/class/cdc/
                     ${CMAKE_CURRENT_LIST_DIR}/usb/class/cdc/device)

  target_sources(
    ${TARGET}
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/usb/class/cdc/device/udi_cdc.c
           ${CMAKE_CURRENT_LIST_DIR}/usb/class/cdc/device/udi_cdc_desc.c)
else()

  message(FATAL_ERROR "Unsupported USB_CLASS: ${USB_CLASS}")

endif()
