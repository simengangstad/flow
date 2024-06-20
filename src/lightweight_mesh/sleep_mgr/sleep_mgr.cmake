include_guard()

include(sleepmgr)

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/inc)

if(${MCU_FAMILY} STREQUAL "XMEGA")

  target_sources(${TARGET}
                 PRIVATE ${CMAKE_CURRENT_LIST_DIR}/xmega_a3u/sleep_mgr.c)

elseif(${MCU_FAMILY} STREQUAL "MEGA")

  include(mac_symbol_counter)

  target_sources(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/megarf/sleep_mgr.c)

else()

  message(FATAL_ERROR "Unsupported MCU family: ${MCU_FAMILY}")

endif()
