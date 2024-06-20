include_guard()

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/inc)

include(common_hw_timer)

target_sources(
  ${TARGET}
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src/sys.c
         ${CMAKE_CURRENT_LIST_DIR}/src/sysEncrypt.c
         ${CMAKE_CURRENT_LIST_DIR}/src/sysTimer.c)
