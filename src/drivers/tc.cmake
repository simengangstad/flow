include_guard()

message(STATUS "Adding Timer/Counter driver")

if(${MCU_FAMILY} STREQUAL "XMEGA")

  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_CURRENT_LIST_DIR}/tc/xmega)

  target_sources(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/tc/xmega/tc.c)

elseif(${MCU_FAMILY} STREQUAL "MEGA")

  target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/tc/mega)

  target_sources(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/tc/mega/tc_megarf.c)

elseif(${MCU_FAMILY} STREQUAL "SAMD20")

  target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/tc/sam0)

  target_sources(
    ${TARGET}
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/tc/sam0/tc_interrupt.c
           ${CMAKE_CURRENT_LIST_DIR}/tc/sam0/tc_sam_d_r_h/tc_interrupt.c)

else()

  message(FATAL_ERROR "Unsupported MCU family: ${MCU_FAMILY}")

endif()
