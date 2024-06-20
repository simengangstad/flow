include_guard()

message(STATUS "Adding watchdog driver")

if(${MCU_FAMILY} STREQUAL "XMEGA")

  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_CURRENT_LIST_DIR}/wdt/xmega)

  target_sources(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/wdt/xmega/wdt.c)

elseif(${MCU_FAMILY} STREQUAL "MEGA")

  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_CURRENT_LIST_DIR}/wdt/mega)

  target_sources(${TARGET}
                 PUBLIC ${CMAKE_CURRENT_LIST_DIR}/wdt/mega/wdt_megarf.c)

else()

  message(FATAL_ERROR "Unsupported MCU family: ${MCU_FAMILY}")

endif()
