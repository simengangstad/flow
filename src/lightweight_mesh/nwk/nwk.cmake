include_guard()

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/inc)

target_sources(
  ${TARGET}
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src/nwk.c
         ${CMAKE_CURRENT_LIST_DIR}/src/nwkDataReq.c
         ${CMAKE_CURRENT_LIST_DIR}/src/nwkFrame.c
         ${CMAKE_CURRENT_LIST_DIR}/src/nwkGroup.c
         ${CMAKE_CURRENT_LIST_DIR}/src/nwkRoute.c
         ${CMAKE_CURRENT_LIST_DIR}/src/nwkRouteDiscovery.c
         ${CMAKE_CURRENT_LIST_DIR}/src/nwkRx.c
         ${CMAKE_CURRENT_LIST_DIR}/src/nwkSecurity.c
         ${CMAKE_CURRENT_LIST_DIR}/src/nwkTx.c)
