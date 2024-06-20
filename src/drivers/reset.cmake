include_guard()

message(STATUS "Adding reset driver")

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/reset)
