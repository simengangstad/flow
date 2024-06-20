include_guard()

message(STATUS "Adding non-volatile memory driver")

if(${MCU_FAMILY} STREQUAL "XMEGA")

  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_CURRENT_LIST_DIR}/nvm/xmega)

  target_sources(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/nvm/xmega/nvm.c
                                  ${CMAKE_CURRENT_LIST_DIR}/nvm/xmega/nvm_asm.S)

  set_property(SOURCE ${CMAKE_CURRENT_LIST_DIR}/nvm/xmega/nvm_asm.S
               PROPERTY LANGUAGE ASM)

else()

  message(FATAL_ERROR "Unsupported MCU family: ${MCU_FAMILY}")

endif()
