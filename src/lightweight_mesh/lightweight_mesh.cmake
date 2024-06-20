include_guard()

message(STATUS "Adding Lightweight Mesh")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_LIST_DIR}/nwk
    ${CMAKE_CURRENT_LIST_DIR}/phy
    ${CMAKE_CURRENT_LIST_DIR}/sal
    ${CMAKE_CURRENT_LIST_DIR}/sleep_mgr
    ${CMAKE_CURRENT_LIST_DIR}/sys
    ${CMAKE_CURRENT_LIST_DIR}/../../drivers)

include(nwk)
include(phy)
include(sal)
include(sleep_mgr)
include(sys)

list(POP_BACK CMAKE_MESSAGE_INDENT)
