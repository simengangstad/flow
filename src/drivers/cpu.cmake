include_guard()

message(STATUS "Adding CPU driver for MCU family: ${MCU_FAMILY}")

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/cpu)

if(${MCU_FAMILY} STREQUAL "XMEGA")
  target_include_directories(
    ${TARGET}
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/cpu/xmega
           ${CMAKE_CURRENT_LIST_DIR}/cpu/xmega/assembler
           ${CMAKE_CURRENT_LIST_DIR}/cpu/xmega/bit_handling
           ${CMAKE_CURRENT_LIST_DIR}/cpu/xmega/preprocessor)

  target_sources(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/cpu/xmega/ccp.S)

  set_property(SOURCE ${CMAKE_CURRENT_LIST_DIR}/cpu/xmega/ccp.S
               PROPERTY LANGUAGE ASM)

elseif(${MCU_FAMILY} STREQUAL "MEGA")
  target_include_directories(
    ${TARGET}
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/cpu/mega
           ${CMAKE_CURRENT_LIST_DIR}/cpu/mega/assembler
           ${CMAKE_CURRENT_LIST_DIR}/cpu/mega/preprocessor)

elseif(${MCU_FAMILY} STREQUAL "SAMD20")
  target_include_directories(
    ${TARGET}
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/cpu/sam0
           ${CMAKE_CURRENT_LIST_DIR}/cpu/sam0/cmsis
           ${CMAKE_CURRENT_LIST_DIR}/cpu/sam0/cmsis/samd20/include
           ${CMAKE_CURRENT_LIST_DIR}/cpu/sam0/cmsis/samd20/source
           ${CMAKE_CURRENT_LIST_DIR}/cpu/sam0/header_files
           ${CMAKE_CURRENT_LIST_DIR}/cpu/sam0/preprocessor)

  target_sources(
    ${TARGET}
    PUBLIC
      ${CMAKE_CURRENT_LIST_DIR}/cpu/interrupt/interrupt_sam_nvic.c
      ${CMAKE_CURRENT_LIST_DIR}/cpu/sam0/cmsis/samd20/source/gcc/startup_samd20.c
      ${CMAKE_CURRENT_LIST_DIR}/cpu/sam0/syscalls/gcc/syscalls.c)

  target_compile_definitions(${TARGET} PUBLIC __SAMD20J18__
                                              ARM_MATH_CM0PLUS=true)

  target_link_options(
    ${TARGET}
    PUBLIC
    -L${CMAKE_CURRENT_LIST_DIR}/cpu/sam0/cmsis/lib
    -T${CMAKE_CURRENT_LIST_DIR}/cpu/sam0/linker_scripts/samd20/gcc/samd20j18_flash.ld
  )

  target_link_libraries(${TARGET} PUBLIC arm_cortexM0l_math m)

  get_target_property(SDK_SOURCES_NO_WARNINGS ${TARGET} SOURCES)

  list(
    FILTER
    SDK_SOURCES_NO_WARNINGS
    INCLUDE
    REGEX
    "\\${CMAKE_CURRENT_LIST_DIR}/cpu/sam0/cmsis/samd20/source/gcc/startup_samd20.c"
  )

  set_source_files_properties(${SDK_SOURCES_NO_WARNINGS}
                              PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS} -w")

else()
  message(FATAL_ERROR "Unsupported MCU family: ${MCU_FAMILY}")
endif()
