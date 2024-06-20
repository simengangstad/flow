include_guard()

message(STATUS "Adding common HW timer driver")

target_include_directories(${TARGET}
                           PUBLIC ${CMAKE_CURRENT_LIST_DIR}/common_hw_timer)

if(${MCU_FAMILY} STREQUAL "XMEGA")

  include(pmic)

  target_include_directories(
    ${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/common_hw_timer/xmega)

  target_sources(
    ${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/common_hw_timer/xmega/hw_timer.c)

  target_link_libraries(
    ${TARGET}
    PUBLIC
      ${CMAKE_CURRENT_LIST_DIR}/common_hw_timer/xmega/lib/libxmegaa3_hw_timer_lib.a
  )

elseif(${MCU_FAMILY} STREQUAL "MEGA")

  target_include_directories(
    ${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/common_hw_timer/mega)

  target_sources(
    ${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/common_hw_timer/mega/hw_timer.c)

  target_link_libraries(
    ${TARGET}
    PUBLIC
      ${CMAKE_CURRENT_LIST_DIR}/common_hw_timer/mega/lib/libmegarfr2_lib_hw_timer.a
  )

elseif(${MCU_FAMILY} STREQUAL "SAMD20")

  target_include_directories(
    ${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/common_hw_timer/sam0)

  target_sources(
    ${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/common_hw_timer/sam0/hw_timer.c)

  target_link_libraries(
    ${TARGET} PUBLI
    ${CMAKE_CURRENT_LIST_DIR}/common_hw_timer/sam0/lib/libsam0_lib_hw_timer.a)

else()

  message(FATAL_ERROR "Unsupported MCU family: ${MCU_FAMILY}")

endif()

include(tc)
