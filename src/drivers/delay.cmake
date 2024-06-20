include_guard()

message(STATUS "Adding delay driver")

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/delay)

if(${MCU_FAMILY} STREQUAL "XMEGA")

  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_CURRENT_LIST_DIR}/delay/xmega)

elseif(${MCU_FAMILY} STREQUAL "MEGA")

  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_CURRENT_LIST_DIR}/delay/mega)

elseif(${MCU_FAMILY} STREQUAL "SAMD20")

  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_CURRENT_LIST_DIR}/delay/sam0)

  target_sources(${TARGET}
                 PUBLIC ${CMAKE_CURRENT_LIST_DIR}/delay/sam0/systick_counter.c)

  target_compile_definitions(${TARGET} PUBLIC SYSTICK_MOD)

else()

  message(FATAL_ERROR "Unsupported MCU family: ${MCU_FAMILY}")

endif()
