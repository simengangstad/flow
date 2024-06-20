include_guard()

message(STATUS "Adding PHY service with radio: ${CONFIG_RADIO}")

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/inc)

if(NOT DEFINED CONFIG_RADIO)
  message(FATAL_ERROR "CONFIG_RADIO not specified!")
endif()

if(${CONFIG_RADIO} STREQUAL "AT86RF233")

  target_include_directories(
    ${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}
                     ${CMAKE_CURRENT_LIST_DIR}/at86rf233/inc)

  target_sources(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/at86rf233/src/phy.c
                                  ${CMAKE_CURRENT_LIST_DIR}/trx_access.c)

  include(spi_master)

elseif(${CONFIG_RADIO} STREQUAL "ATMEGARFR2")

  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_CURRENT_LIST_DIR}/atmegarfr2/inc)

  target_sources(${TARGET}
                 PUBLIC ${CMAKE_CURRENT_LIST_DIR}/atmegarfr2/src/phy.c)

elseif(${CONFIG_RADIO} STREQUAL "AT86RF231")

  target_include_directories(
    ${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}
                     ${CMAKE_CURRENT_LIST_DIR}/at86rf231/inc)

  target_sources(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/at86rf233/src/phy.c
                                  ${CMAKE_CURRENT_LIST_DIR}/trx_access.c)

  include(spi_master)

else()

  message(FATAL_ERROR "Unsupported CONFIG_RADIO: ${CONFIG_RADIO}")

endif()
