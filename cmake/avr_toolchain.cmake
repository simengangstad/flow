# The toolchain expects that TARGET is set to the target binary (e.g.,
# myapplication). It also expects that SOURCES is set with the source file for
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
find_program(AVR_CC avr-gcc REQUIRED)
find_program(AVR_CXX avr-g++ REQUIRED)
find_program(AVR_AS avr-gcc REQUIRED)
find_program(AVR_OBJCOPY avr-objcopy REQUIRED)
find_program(AVR_OBJDUMP avr-objdump REQUIRED)
find_program(AVR_STRIP avr-strip REQUIRED)

if(TOOLCHAIN_USE_PYMCUPROG)
  find_program(PROGRAMMER pymcuprog REQUIRED)
elseif(TOOLCHAIN_USE_DFU_PROGRAMMER)
  find_program(PROGRAMMER dfu-programmer REQUIRED)
else()
  find_program(PROGRAMMER avrdude REQUIRED)
endif()

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)

set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_COMPILER ${AVR_CC})

set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_COMPILER ${AVR_CXX})

set(CMAKE_ASM_COMPILER ${AVR_AS})

set(CMAKE_OBJCOPY ${AVR_OBJCOPY})
set(CMAKE_OBJDUMP ${AVR_OBJDUMP})
set(CMAKE_STRIP ${AVR_STRIP})

# Export compile commands for use with LSPs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_language(C CXX ASM)

# ------------------------------- Device pack --------------------------------

# Find the device pack if specified (this is needed for newer devices which
# don't have native support in avr-gcc)
if(TOOLCHAIN_NEED_DEVICE_PACK)

  set(DEVICE_PACK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/device_pack)

  if(NOT EXISTS ${DEVICE_PACK_DIR})
    message(
      FATAL_ERROR
        "Device pack not found in root of project directory. The device pack (ATPACK/DFP) should be in a folder named 'device_pack'. Download it for your device at http://packs.download.atmel.com."
    )
  endif()

  # Helper variables for the device related directories in the ATPACK and the
  # toolchain
  set(MCU_DEVICE_DIRECTORY ${DEVICE_PACK_DIR}/gcc/dev/${MCU})
  set(MCU_INCLUDE_DIRECTORY ${DEVICE_PACK_DIR}/include)

  if(NOT EXISTS ${MCU_DEVICE_DIRECTORY}/device-specs/specs-${MCU})
    message(
      FATAL_ERROR
        "Could not find device pack for ${MCU}, tried looking in directory ${MCU_DEVICE_DIRECTORY}/device-specs/specs-${MCU}. The device name might be misspelled or the device pack might be incorrect for this device."
    )
  endif()

  # Find the device library name by going through the device specs for the
  # device
  file(READ ${MCU_DEVICE_DIRECTORY}/device-specs/specs-${MCU}
       SPECS_FILE_CONTENT)
  string(
    REGEX MATCH
          "-D__AVR_DEVICE_NAME__=${MCU} -D__AVR_DEV_LIB_NAME__=([a-zA-Z0-9]*)"
          _ ${SPECS_FILE_CONTENT})
  set(MCU_DEV_LIB_NAME ${CMAKE_MATCH_1})

endif()

# ---------------------------- Definitions & flags -----------------------------

if(NOT DEFINED F_CPU)
  message(WARNING "F_CPU not defined, has to be defined in code")
else()
  set(TOOLCHAIN_COMPILE_DEFINITIONS F_CPU=${F_CPU})
endif()

set(TOOLCHAIN_COMPILE_DEFINITIONS
    ${TOOLCHAIN_COMPILE_DEFINITIONS} $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>)

set(TOOLCHAIN_COMPILE_OPTIONS
    -mmcu=${MCU}
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wno-array-bounds
    -Wno-vla
    # ASF errors
    -Wno-expansion-to-defined
    -Wno-int-in-bool-context
    # Optimisations
    -funsigned-char
    -funsigned-bitfields
    -fshort-enums
    -ffunction-sections
    -fdata-sections
    -fno-split-wide-types
    -fno-tree-scev-cprop
    $<$<CONFIG:Debug>:-Og>
    $<$<CONFIG:Debug>:-Wno-unused-function>
    $<$<CONFIG:Release>:-Os>
    $<$<CONFIG:Release>:-Werror>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-volatile>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-register>)

set(TOOLCHAIN_LINK_OPTIONS
    -mmcu=${MCU}
    -Wl,-Map=${TARGET}.map
    -Wl,--print-memory-usage
    -Wl,--gc-section
    -Wl,--sort-section=alignment
    -Wl,--cref
    $<$<CONFIG:Release>:-Os>
    $<$<CONFIG:Debug>:-Og>)

if(TOOLCHAIN_NEED_DEVICE_PACK)
  set(TOOLCHAIN_COMPILE_DEFINITIONS ${TOOLCHAIN_COMPILE_DEFINITIONS}
                                    __AVR_DEV_LIB_NAME__=${MCU_DEV_LIB_NAME})

  set(TOOLCHAIN_COMPILE_OPTIONS
      ${TOOLCHAIN_COMPILE_OPTIONS}
      # Include the AVR header files from the ATPACK
      -I${MCU_INCLUDE_DIRECTORY}
      # Notify the compiler about the device specs
      -B${MCU_DEVICE_DIRECTORY})

  set(TOOLCHAIN_LINK_OPTIONS
      ${TOOLCHAIN_LINK_OPTIONS}
      # Notify the compiler about the device specs
      -B${MCU_DEVICE_DIRECTORY})

endif()

add_executable(${TARGET} ${SOURCES})

target_compile_options(${TARGET} PRIVATE ${TOOLCHAIN_COMPILE_OPTIONS})
target_link_options(${TARGET} PRIVATE ${TOOLCHAIN_LINK_OPTIONS})
target_compile_definitions(${TARGET} PRIVATE ${TOOLCHAIN_COMPILE_DEFINITIONS})

set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME ${TARGET}.elf)

# HEX
add_custom_target(
  hex ALL
  ${CMAKE_OBJCOPY} -O ihex ${TARGET}.elf ${TARGET}.hex
  DEPENDS ${TARGET}
  COMMENT "Creating ${TARGET}.hex")

# EEPROM
add_custom_target(
  eeprom ALL
  ${CMAKE_OBJCOPY}
  -j
  .eeprom
  --set-section-flags=.eeprom=alloc,load
  --change-section-lma
  .eeprom=0
  --no-change-warnings
  -O
  ihex
  ${TARGET}.elf
  ${TARGET}-eeprom.hex
  DEPENDS ${TARGET}
  COMMENT "Creating ${TARGET}-eeprom.hex")

if(TOOLCHAIN_USE_PYMCUPROG)

  message(STATUS "Using pymcuprog as programmer")

  add_custom_target(
    flash
    ${PROGRAMMER} write -f ${TARGET}.hex --erase --verify
    DEPENDS hex
    COMMENT "Flashing ${TARGET}.hex to ${MCU}")

  add_custom_target(
    reset
    ${PROGRAMMER} reset
    COMMENT "Resetting device...")

elseif(TOOLCHAIN_USE_DFU_PROGRAMMER)

  message(STATUS "Using dfu-programmer as programmer")

  add_custom_target(
    flash
    ${PROGRAMMER} ${PROGRAMMER_MCU} flash ${TARGET}.hex --force --ignore-outside
    DEPENDS hex
    COMMENT "Flashing ${TARGET}.hex to ${MCU}")

  add_custom_target(
    start
    ${PROGRAMMER} ${PROGRAMMER_MCU} start
    COMMENT "Starting application...")

else()

  message(STATUS "Using avrdude as programmer with: ${PROGRAMMER_ID}")

  add_custom_target(
    flash
    ${PROGRAMMER}
    -c
    ${PROGRAMMER_ID}
    -p
    ${PROGRAMMER_MCU}
    -B
    0.125
    -U
    flash:w:${TARGET}.hex:i
    DEPENDS hex
    COMMENT "Flashing ${TARGET}.hex to ${MCU}")

  add_custom_target(
    flash_eeprom
    ${PROGRAMMER}
    -c
    ${PROGRAMMER_ID}
    -p
    ${PROGRAMMER_MCU}
    -U
    eeprom:w:${TARGET}-eeprom.hex:i
    DEPENDS eeprom
    COMMENT "Uploading ${TARGET}-eeprom.hex to ${MCU}")

  add_custom_target(
    reset
    ${PROGRAMMER} -c ${PROGRAMMER_ID} -p ${PROGRAMMER_MCU}
    COMMENT "Resetting ${MCU}")

  add_custom_target(
    read_fuses
    ${PROGRAMMER}
    -c
    ${PROGRAMMER_ID}
    -p
    ${PROGRAMMER_MCU}
    -U
    hfuse:r:-:h
    -U
    lfuse:r:-:h
    COMMENT "Reading fuses from ${MCU}...")

endif()

# Disassemble
add_custom_target(
  disassemble
  ${CMAKE_OBJDUMP} -h -S ${TARGET}.elf > ${TARGET}.lst
  DEPENDS strip
  COMMENT "Disassembling ${TARGET}.elf")

# Remove .hex, .map, eeprom .hex and .lst on clean
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
  PROPERTY ADDITIONAL_CLEAN_FILES ${TARGET}-eeprom.hex)

set_property(
  TARGET ${TARGET}
  APPEND
  PROPERTY ADDITIONAL_CLEAN_FILES ${TARGET}.lst)
