include_guard()

message(STATUS "Adding clock driver")

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/clock)

if(${MCU_FAMILY} STREQUAL "XMEGA")

  include(nvm)

  target_sources(${TARGET}
                 PUBLIC ${CMAKE_CURRENT_LIST_DIR}/clock/xmega/sysclk.c)

elseif(${MCU_FAMILY} STREQUAL "MEGA")

  target_sources(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/clock/mega/sysclk.c)

else()

  message(FATAL_ERROR "Unsupported MCU family: ${MCU_FAMILY}")

endif()
