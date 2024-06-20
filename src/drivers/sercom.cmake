include_guard()

message(STATUS "Adding sercom driver")

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/serial)

if(${MCU_FAMILY} STREQUAL "SAMD20")

  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_CURRENT_LIST_DIR}/sercom/sam0)

  target_sources(
    ${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/sercom/sam0/sercom.c
                     ${CMAKE_CURRENT_LIST_DIR}/sercom/sercom_interrupt.c)

else()

  message(FATAL_ERROR "Unsupported MCU family: ${MCU_FAMILY}")

endif()
