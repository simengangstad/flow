# The toolchain expects that TARGET is set to the target binary (e.g.,
# myapplication). It also # expects that SOURCES is set with the source file for
# the project (can be genrated with file(GLOB SOURCES <your sources>))

if(NOT DEFINED TARGET)
  message(
    FATAL
    "TARGET variable is not set. Needs to be set before including the toolchain."
  )
endif()

if(NOT DEFINED SOURCES)
  message(
    FATAL
    "SOURCES variable is not set. Needs to be set before including the toolchain."
  )
endif()

set(DEFAULT_BUILD_TYPE "Release")

if(NOT CMAKE_BUILD_TYPE)
  message(
    STATUS
      "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}")
endif()

# Find the binaries for the toolchain
find_program(ARM_CC arm-none-eabi-gcc REQUIRED)
find_program(ARM_CXX arm-none-eabi-g++ REQUIRED)
find_program(ARM_AS arm-none-eabi-gcc REQUIRED)
find_program(ARM_OBJCOPY arm-none-eabi-objcopy REQUIRED)
find_program(ARM_OBJDUMP arm-none-eabi-objdump REQUIRED)

find_program(PROGRAMMER openocd REQUIRED)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_COMPILER ${ARM_CC})

set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_COMPILER ${ARM_CXX})

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_ASM_COMPILER ${ARM_AS})

set(CMAKE_OBJCOPY ${ARM_OBJCOPY})
set(CMAKE_OBJDUMP ${AVR_OBJDUMP})
set(CMAKE_STRIP ${ARM_STRIP})

# Export compile commands for use with LSPs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_language(C CXX ASM)

# ---------------------------- Definitions & flags -----------------------------

set(TOOLCHAIN_COMPILE_DEFINITIONS
    ${TOOLCHAIN_COMPILE_DEFINITIONS} $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>)

set(TOOLCHAIN_COMPILE_OPTIONS
    -mcpu=${MCU}
    -mthumb
    -mlong-calls
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wno-array-bounds
    -Wno-vla
    # # ASF errors
    -Wno-expansion-to-defined
    -Wno-int-in-bool-context
    # # Optimisations
    -funsigned-char
    -funsigned-bitfields
    -fshort-enums
    -ffunction-sections
    -fdata-sections
    -fno-split-wide-types
    -fno-tree-scev-cprop
    $<$<CONFIG:Debug>:-O0>
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Debug>:-Wno-unused-function>
    $<$<CONFIG:Release>:-Os>
    $<$<CONFIG:Release>:-Werror>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-volatile>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-register>
)

set(TOOLCHAIN_LINK_OPTIONS
    -mcpu=${MCU}
    -Wl,-Map=${TARGET}.map
    -Wl,--print-memory-usage
    -Wl,--gc-section
    -Wl,--sort-section=alignment
    -Wl,--cref
    --specs=nano.specs
    $<$<CONFIG:Release>:-Os>
    $<$<CONFIG:Debug>:-Og>
    $<$<CONFIG:Debug>:-g>)

add_executable(${TARGET} ${SOURCES})

target_compile_options(${TARGET} PRIVATE ${TOOLCHAIN_COMPILE_OPTIONS})
target_link_options(${TARGET} PRIVATE ${TOOLCHAIN_LINK_OPTIONS})
target_compile_definitions(${TARGET} PRIVATE ${TOOLCHAIN_COMPILE_DEFINITIONS})

set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME ${TARGET}.elf)

# STRIP add_custom_target( strip ALL ${CMAKE_STRIP} ${TARGET}.elf DEPENDS
# ${TARGET} COMMENT "Stripping ${TARGET}.elf")
#
# # HEX add_custom_target( hex ALL ${CMAKE_OBJCOPY} -O ihex ${TARGET}.elf
# ${TARGET}.hex DEPENDS strip COMMENT "Creating ${TARGET}.hex")

add_custom_target(
  hex ALL
  ${CMAKE_OBJCOPY} -O ihex ${TARGET}.elf ${TARGET}.hex
  DEPENDS ${TARGET}
  COMMENT "Creating ${TARGET}.hex")

add_custom_target(
  flash
  ${PROGRAMMER}
  -f
  ${PROGRAMMER_ID}
  -c
  "program ${TARGET}.hex verify"
  -c
  "reset run"
  -c
  "exit"
  DEPENDS hex
  COMMENT "Flashing ${TARGET}.hex to ${MCU}")

add_custom_target(
  reset
  ${PROGRAMMER}
  -f
  ${PROGRAMMER_ID}
  -c
  "init"
  -c
  "reset run"
  -c
  "exit"
  COMMENT "Resetting ${MCU}")

# Disassemble
add_custom_target(
  disassemble
  ${CMAKE_OBJDUMP} -h -S ${TARGET}.elf > ${TARGET}.lst
  DEPENDS strip
  COMMENT "Disassembling ${TARGET}.elf")

# Remove .hex, .map, .hex and .lst on clean
set_property(
  TARGET ${TARGET}
  APPEND
  PROPERTY ADDITIONAL_CLEAN_FILES ${TARGET}.hex)

set_property(
  TARGET ${TARGET}
  APPEND
  PROPERTY ADDITIONAL_CLEAN_FILES ${TARGET}.map)

set_property(
  TARGET ${TARGET}
  APPEND
  PROPERTY ADDITIONAL_CLEAN_FILES ${TARGET}.lst)
