include_guard()

message(STATUS "Adding SPI master driver")

include(spi)

target_include_directories(${TARGET}
                           PUBLIC ${CMAKE_CURRENT_LIST_DIR}/spi/master)

if(${MCU_FAMILY} STREQUAL "XMEGA")

  target_sources(
    ${TARGET}
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/spi/master/xmega_spi/spi_master.c)

elseif(${MCU_FAMILY} STREQUAL "MEGA")

  target_sources(
    ${TARGET}
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/spi/master/megarf_spi/spi_master.c)

else()

  message(FATAL_ERROR "Unsupported MCU family: ${MCU_FAMILY}")

endif()
