include_guard()

message(STATUS "Adding SPI driver")

if(${MCU_FAMILY} STREQUAL "XMEGA")

  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_CURRENT_LIST_DIR}/spi/xmega)

  target_sources(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/spi/xmega/spi.c)

elseif(${MCU_FAMILY} STREQUAL "MEGA")

  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_CURRENT_LIST_DIR}/spi/mega)

  target_sources(${TARGET}
                 PUBLIC ${CMAKE_CURRENT_LIST_DIR}/spi/mega/spi_megarf.c)

elseif(${MCU_FAMILY} STREQUAL "SAMD20")

  include(sercom)

  target_compile_definitions(${TARGET} PUBLIC SPI_CALLBACK_MODE=true)

  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_CURRENT_LIST_DIR}/spi/sam0)

  target_sources(
    ${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/spi/sam0/spi.c
                     ${CMAKE_CURRENT_LIST_DIR}/spi/sam0/spi_interrupt.c)
else()

  message(FATAL_ERROR "Unsupported MCU family: ${MCU_FAMILY}")

endif()
