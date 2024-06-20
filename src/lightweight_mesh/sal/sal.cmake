include_guard()

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/inc)

if(${CONFIG_RADIO} MATCHES "AT86RF2")

  target_sources(${TARGET}
                 PRIVATE ${CMAKE_CURRENT_LIST_DIR}/at86rf2xx/src/sal.c)

elseif(${CONFIG_RADIO} STREQUAL "ATMEGARFR2")

  target_sources(${TARGET}
                 PRIVATE ${CMAKE_CURRENT_LIST_DIR}/atmegarfr2/src/sal.c)

else()

  message(FATAL_ERROR "Unsupported CONFIG_RADIO: ${CONFIG_RADIO}")

endif()
