include_guard()

message(STATUS "Adding port driver")

if(${MCU_FAMILY} STREQUAL "SAMD20")

  target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/port)

  target_sources(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/port/sam0/port.c)

else()

  message(FATAL_ERROR "Unsupported MCU family: ${MCU_FAMILY}")

endif()
