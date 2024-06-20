include_guard()

message(STATUS "Adding serial driver")

include(usart)

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/serial)
target_sources(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/serial/usart_serial.c)

if(${MCU_FAMILY} STREQUAL "XMEGA")

  include(pmic)

elseif(${MCU_FAMILY} STREQUAL "MEGA")

  # pass

elseif(${MCU_FAMILY} STREQUAL "SAMD20")

  target_compile_definitions(${TARGET} PUBLIC USART_CALLBACK_MODE=true)

else()

  message(FATAL_ERROR "Unsupported MCU family: ${MCU_FAMILY}")

endif()
