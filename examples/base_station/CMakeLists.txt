cmake_minimum_required(VERSION 3.20)

# -------------------------------- Configuration -------------------------------

set(TARGET base_station)

set(MCU atmega256rfr2)
set(MCU_FAMILY MEGA)
set(BOARD ATMEGA256RFR2_XPLAINED_PRO)
set(F_CPU 8000000)

set(PROGRAMMER_MCU m256rfr2)
set(PROGRAMMER_ID xplainedpro)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/)

# ----------------------------------- Target -----------------------------------

file(GLOB SOURCES src/example_base_station.cpp)
include(avr_toolchain)

project(base_station)

include(flow)
include(wdt)

include(file_definitions)
add_definitions_from_file(${TARGET} ${CMAKE_CURRENT_LIST_DIR}/../env)

target_compile_definitions(${TARGET} PRIVATE -DMESH_ENABLE_LOGGING)
