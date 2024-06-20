include_guard()

message(STATUS "Adding USART driver")

if(${MCU_FAMILY} STREQUAL "XMEGA")

  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_CURRENT_LIST_DIR}/usart/xmega)

  target_sources(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/usart/xmega/usart.c)

elseif(${MCU_FAMILY} STREQUAL "MEGA")

  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_CURRENT_LIST_DIR}/usart/mega)

  target_sources(${TARGET}
                 PUBLIC ${CMAKE_CURRENT_LIST_DIR}/usart/mega/usart_megarf.c)

elseif(${MCU_FAMILY} STREQUAL "SAMD20")

  include(sercom)

  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_CURRENT_LIST_DIR}/usart/sam0)

  target_sources(
    ${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/usart/sam0/usart.c
                     ${CMAKE_CURRENT_LIST_DIR}/usart/sam0/usart_interrupt.c)
else()

  message(FATAL_ERROR "Unsupported MCU family: ${MCU_FAMILY}")

endif()
