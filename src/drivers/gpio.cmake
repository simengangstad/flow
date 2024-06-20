include_guard()

message(STATUS "Adding GPIO driver")

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/gpio
                                            ${CMAKE_CURRENT_LIST_DIR}/ioport)

if(${MCU_FAMILY} STREQUAL "XMEGA")

  target_sources(${TARGET}
                 PUBLIC ${CMAKE_CURRENT_LIST_DIR}/ioport/xmega/ioport_compat.c)

elseif(${MCU_FAMILY} STREQUAL "MEGA")

  # Pass

elseif(${MCU_FAMILY} STREQUAL "SAMD20")

  # Pass

else()

  message(FATAL_ERROR "Unsupported MCU family: ${MCU_FAMILY}")

endif()
