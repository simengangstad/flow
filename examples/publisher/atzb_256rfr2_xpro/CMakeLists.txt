cmake_minimum_required(VERSION 3.20)

# -------------------------------- Configuration -------------------------------

set(TARGET publisher)

set(MCU atmega256rfr2)
set(MCU_FAMILY MEGA)
set(BOARD ATMEGA256RFR2_ZIGBIT)
set(F_CPU 8000000)

set(PROGRAMMER_ID powerdebugger)
set(PROGRAMMER_MCU m256rfr2)

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
