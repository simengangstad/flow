include_guard()

message(STATUS "Adding external interrupt driver")

if(${MCU_FAMILY} STREQUAL "MEGA")

  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_CURRENT_LIST_DIR}/extint/mega)

  target_sources(${TARGET}
                 PUBLIC ${CMAKE_CURRENT_LIST_DIR}/extint/mega/extint_megarf.c)

elseif(${MCU_FAMILY} STREQUAL "SAMD20")

  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_CURRENT_LIST_DIR}/extint/sam0)

  target_sources(
    ${TARGET}
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/extint/sam0/extint_callback.c
           ${CMAKE_CURRENT_LIST_DIR}/extint/sam0/extint_sam_d_r_h/extint.c)

else()

  message(FATAL_ERROR "Unsupported MCU family: ${MCU_FAMILY}")

endif()
