include_guard()

message(STATUS "Adding serial I/O to host driver")

list(APPEND CMAKE_MESSAGE_INDENT "  ")

if(NOT DEFINED CONFIG_SIO2HOST_MODE)
  message(FATAL_ERROR "CONFIG_SIO2HOST_MODE not specified!")
endif()

if(${CONFIG_SIO2HOST_MODE} STREQUAL "UART")

  set(CONFIG_STDIO_MODE "SERIAL")
  include(serial)
  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_CURRENT_LIST_DIR}/sio2host/uart)

  target_sources(${TARGET}
                 PUBLIC ${CMAKE_CURRENT_LIST_DIR}/sio2host/uart/sio2host.c)

elseif(${CONFIG_SIO2HOST_MODE} STREQUAL "USB")
  set(CONFIG_STDIO_MODE "USB")
  include(usb)
  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_CURRENT_LIST_DIR}/sio2host/usb)

  target_sources(${TARGET}
                 PUBLIC ${CMAKE_CURRENT_LIST_DIR}/sio2host/usb/sio2host.c)

else()

  message(
    FATAL_ERROR "Unsupported CONFIG_SIO2HOST_MODE: ${CONFIG_SIO2HOST_MODE}")

endif()

include(stdio)

list(POP_BACK CMAKE_MESSAGE_INDENT)
