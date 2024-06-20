include_guard()

message(STATUS "Adding PMIC driver")

if(${MCU_FAMILY} STREQUAL "XMEGA")

  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_CURRENT_LIST_DIR}/pmic/xmega)

else()

  message(FATAL_ERROR "Unsupported MCU family: ${MCU_FAMILY}")

endif()
