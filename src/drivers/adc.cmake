include_guard()

message(STATUS "Adding ADC driver")

if(${MCU_FAMILY} STREQUAL "XMEGA")

  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_CURRENT_LIST_DIR}/adc/xmega)

  target_sources(
    ${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/adc/xmega/adc.c
                     ${CMAKE_CURRENT_LIST_DIR}/adc/xmega/xmega_aau/adc_aau.c)

elseif(${MCU_FAMILY} STREQUAL "MEGA")

  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_CURRENT_LIST_DIR}/adc/mega)

  target_sources(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/adc/mega/adc.c)

else()

  message(FATAL_ERROR "Unsupported MCU family: ${MCU_FAMILY}")

endif()
