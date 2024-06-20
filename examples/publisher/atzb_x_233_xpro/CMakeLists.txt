cmake_minimum_required(VERSION 3.20)

# -------------------------------- Configuration -------------------------------

set(TARGET publisher)

set(MCU atxmega256a3u)
set(MCU_FAMILY XMEGA)
set(BOARD XMEGA_RF233_ZIGBIT)
set(RF233_ZIGBIT_TYPE EXT)
set(F_CPU 32000000)

set(PROGRAMMER_ID powerdebugger)
set(PROGRAMMER_MCU atxmega256a3u)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      ${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/)

# ----------------------------------- Target -----------------------------------

file(GLOB SOURCES src/example_publisher.cpp)
include(avr_toolchain)

project(publisher)

include(flow)
include(wdt)

include(file_definitions)
add_definitions_from_file(${TARGET} ${CMAKE_CURRENT_LIST_DIR}/../../env)
