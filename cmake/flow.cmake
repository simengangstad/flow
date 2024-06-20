include_guard()

# ------------------------ Board specific configuration -----------------------

if(${BOARD} STREQUAL "XMEGA_RF233_ZIGBIT")

  set(CONFIG_RADIO "AT86RF233")
  set(MESH_FLAGS -DSAL_TYPE=ATXMEGA_SAL -DIOPORT_XMEGA_COMPAT
                 -DCONFIG_NVM_IGNORE_XMEGA_A3_D3_REVB_ERRATA -DPHY_AT86RF233)

  if(${RF233_ZIGBIT_TYPE} STREQUAL "USB")

    set(CONFIG_SIO2HOST_MODE "USB")
    set(CONFIG_USB_CLASS "CDC")
    set(MESH_FLAGS ${MESH_FLAGS} -DZIGBIT_USB)

  elseif(${RF233_ZIGBIT_TYPE} STREQUAL "EXT")

    set(CONFIG_SIO2HOST_MODE "UART")
    set(MESH_FLAGS ${MESH_FLAGS} -DZIGBIT_EXT)

  else()

    message(
      FATAL_ERROR
        "Unsupported RF233_ZIGBIT_TYPE: ${RF233_ZIGBIT_TYPE}. Supported: USB or EXT."
    )

  endif()

elseif(${BOARD} STREQUAL "ATMEGA256RFR2_XPLAINED_PRO")

  set(CONFIG_SIO2HOST_MODE "UART")
  set(CONFIG_RADIO "ATMEGARFR2")
  set(MESH_FLAGS -DSAL_TYPE=ATMEGARF_SAL -DPHY_ATMEGARFR2)

elseif(${BOARD} STREQUAL "ATMEGA256RFR2_ZIGBIT")

  set(CONFIG_SIO2HOST_MODE "UART")
  set(CONFIG_RADIO "ATMEGARFR2")
  set(MESH_FLAGS -DSAL_TYPE=ATMEGARF_SAL -DPHY_ATMEGARFR2)

elseif(${BOARD} STREQUAL "SAMD20_XPLAINED_PRO")

  set(CONFIG_SIO2HOST_MODE "UART")
  set(CONFIG_RADIO "AT86RF231")
  set(MESH_FLAGS -DSAL_TYPE=AT86RF2xx -DPHY_AT86RF231)

else()

  message(FATAL_ERROR "Unsupported board: ${BOARD}")

endif()

# --------------------------- External Dependencies ---------------------------
include(FetchContent)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(FETCHCONTENT_QUIET FALSE)

include(etl)

message(STATUS "")
message(STATUS "Configuring Flow framework")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

# ------------------------------ Mesh Framework -------------------------------

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../src)

target_sources(
  ${TARGET}
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../src/mesh.cpp
          ${CMAKE_CURRENT_LIST_DIR}/../src/low_power.cpp
          ${CMAKE_CURRENT_LIST_DIR}/../src/publisher.cpp)

set(SRC_PATH ${CMAKE_CURRENT_LIST_DIR}/../src)

# Makes the path absolute and removes ../ from the path. This is needed when
# specifying that we silence warning for the library files, as the path can't
# include ../ in the regex.
get_filename_component(SRC_PATH ${SRC_PATH} REALPATH BASE_DIR
                       ${CMAKE_BINARY_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SRC_PATH}/drivers
                      ${SRC_PATH}/lightweight_mesh)

include(cpu)
include(system)
include(board)
include(clock)
include(delay)
include(gpio)
include(sleepmgr)
include(sio2host)
include(reset)

include(lightweight_mesh)

list(POP_BACK CMAKE_MESSAGE_INDENT)

# Remove warnings from compiled units of the ASF. This will still propagate
# errors when used in source files in the project, but not when the ASF files
# are compiled themselves
get_target_property(ASF_SOURCES_NO_WARNINGS ${TARGET} SOURCES)
list(FILTER ASF_SOURCES_NO_WARNINGS INCLUDE REGEX
     "\\${SRC_PATH}/drivers|\\${SRC_PATH}/lightweight_mesh")
set_source_files_properties(${ASF_SOURCES_NO_WARNINGS}
                            PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS} -w")

# ---------------------------------- TARGET ----------------------------------

target_compile_definitions(${TARGET} PRIVATE ${MESH_FLAGS})
