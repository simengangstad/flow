include_guard()

FetchContent_Declare(
  etl
  GIT_REPOSITORY https://github.com/ETLCPP/etl
  GIT_TAG 20.38.3
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE)

FetchContent_MakeAvailable(etl)

target_compile_definitions(
  ${TARGET}
  PRIVATE -DETL_NO_STL $<$<CONFIG:Debug>:ETL_LOG_ERRORS>
          $<$<CONFIG:Debug>:ETL_VERBOSE_ERRORS>
          $<$<CONFIG:Debug>:ETL_CHECK_PUSH_POP>)
target_link_libraries(${TARGET} PRIVATE etl::etl)
