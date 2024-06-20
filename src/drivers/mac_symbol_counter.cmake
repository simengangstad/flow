include_guard()

message(STATUS "Adding MAC symbol counter driver")

if(${MCU_FAMILY} STREQUAL "MEGA")

  target_include_directories(
    ${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/mac_symbol_counter/mega)

  target_sources(
    ${TARGET}
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/mac_symbol_counter/mega/macsc_megarf.c)

else()

  message(FATAL_ERROR "Unsupported MCU family: ${MCU_FAMILY}")

endif()
