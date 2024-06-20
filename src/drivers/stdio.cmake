include_guard()

message(STATUS "Adding standard I/O driver for mode: ${CONFIG_STDIO_MODE}")

target_sources(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/stdio/read.c
                                ${CMAKE_CURRENT_LIST_DIR}/stdio/write.c)

if(NOT DEFINED CONFIG_STDIO_MODE)
  message(FATAL_ERROR "CONFIG_STDIO_MODE not specified!")
endif()

if(${CONFIG_STDIO_MODE} STREQUAL "SERIAL")

  target_include_directories(
    ${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/stdio/stdio_serial)

elseif(${CONFIG_STDIO_MODE} STREQUAL "USB")

  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_CURRENT_LIST_DIR}/stdio/stdio_usb)

  target_sources(${TARGET}
                 PUBLIC ${CMAKE_CURRENT_LIST_DIR}/stdio/stdio_usb/stdio_usb.c)

else()

  message(
    FATAL_ERROR "Unsupported CONFIG_STDIO_MODE mode: ${CONFIG_STDIO_MODE}")

endif()
