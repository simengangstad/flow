include_guard()

message(STATUS "Adding system driver")

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/system)

if(${MCU_FAMILY} STREQUAL "XMEGA")

  target_sources(${TARGET}
                 PUBLIC ${CMAKE_CURRENT_LIST_DIR}/system/mega/system.c)

elseif(${MCU_FAMILY} STREQUAL "MEGA")

  target_sources(${TARGET}
                 PUBLIC ${CMAKE_CURRENT_LIST_DIR}/system/xmega/system.c)

elseif(${MCU_FAMILY} STREQUAL "SAMD20")

  target_include_directories(
    ${TARGET}
    PUBLIC
      ${CMAKE_CURRENT_LIST_DIR}/system/sam0/clock
      ${CMAKE_CURRENT_LIST_DIR}/system/sam0/clock/clock_samd20
      ${CMAKE_CURRENT_LIST_DIR}/system/sam0/interrupt
      ${CMAKE_CURRENT_LIST_DIR}/system/sam0/interrupt/system_interrupt_samd20
      ${CMAKE_CURRENT_LIST_DIR}/system/sam0/pinmux
      ${CMAKE_CURRENT_LIST_DIR}/system/sam0/power/power_sam_d_r_h)

  target_sources(
    ${TARGET}
    PUBLIC
      ${CMAKE_CURRENT_LIST_DIR}/cpu/sam0/cmsis/samd20/source/system_samd20.c
      ${CMAKE_CURRENT_LIST_DIR}/system/sam0/clock/clock_samd20/clock.c
      ${CMAKE_CURRENT_LIST_DIR}/system/sam0/clock/clock_samd20/gclk.c
      ${CMAKE_CURRENT_LIST_DIR}/system/sam0/interrupt/system_interrupt.c
      ${CMAKE_CURRENT_LIST_DIR}/system/sam0/system.c
      ${CMAKE_CURRENT_LIST_DIR}/system/sam0/pinmux/pinmux.c)

else()

  message(FATAL_ERROR "Unsupported MCU family: ${MCU_FAMILY}")

endif()

include(board)
