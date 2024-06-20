include_guard()

message(STATUS "Adding sleep manager driver")

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/sleepmgr)

if(${MCU_FAMILY} STREQUAL "XMEGA")

  target_sources(${TARGET}
                 PUBLIC ${CMAKE_CURRENT_LIST_DIR}/sleepmgr/xmega/sleepmgr.c)

elseif(${MCU_FAMILY} STREQUAL "MEGA")

  target_include_directories(
    ${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../arch/mega/drivers/sleep)

else()

  message(FATAL_ERROR "Unsupported MCU family: ${MCU_FAMILY}")

endif()

include(sleep)
