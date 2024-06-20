include_guard()

message(STATUS "Adding board driver for: ${BOARD}")

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/board)
target_compile_definitions(${TARGET} PUBLIC BOARD=${BOARD})

if(${BOARD} STREQUAL "ATMEGA256RFR2_XPLAINED_PRO")

  target_include_directories(
    ${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/board/atmega256rfr2_xplained_pro)

  target_sources(
    ${TARGET}
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/board/atmega256rfr2_xplained_pro/init.c)

elseif(${BOARD} STREQUAL "ATMEGA256RFR2_ZIGBIT")

  target_include_directories(
    ${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/board/atmega256rfr2_zigbit)

  target_sources(
    ${TARGET}
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/board/atmega256rfr2_zigbit/init.c)

elseif(${BOARD} STREQUAL "XMEGA_RF233_ZIGBIT")

  target_include_directories(
    ${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/board/xmega_rf233_zigbit)

  target_sources(
    ${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/board/xmega_rf233_zigbit/init.c)

elseif(${BOARD} STREQUAL "SAMD20_XPLAINED_PRO")

  target_include_directories(
    ${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/boards/samd20_xplained_pro)

  target_sources(
    ${TARGET}
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/boards/samd20_xplained_pro/board_init.c)

  include(port)

else()
  message(FATAL_ERROR "Unsupported board: ${BOARD}")
endif()
