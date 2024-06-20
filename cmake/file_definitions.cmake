include_guard()

# add_definitions_from_file extracts preprocessor definitions from @p and
# appends them to @p target
#
# The file should be on the following format:
#
# DEF1=VALUE1
#
# DEF2=VALUE2
#
# DEF3=VALUE3
function(add_definitions_from_file target file)

  file(STRINGS ${file} FILE_CONTENT)

  foreach(line ${FILE_CONTENT})

    if(NOT "${line}" STREQUAL "")
      string(REGEX MATCH "^([^=]+)=(.*)" _ ${line})
      target_compile_definitions(${target}
                                 PRIVATE ${CMAKE_MATCH_1}=${CMAKE_MATCH_2})
    endif()
  endforeach()
endfunction()
