include_directories(ssplit)
add_library(ssplit STATIC ssplit/ssplit.cpp ssplit/regex.cpp)
target_link_libraries(ssplit ${EXT_LIBS} ${ABSEIL_LIBS})
set_target_properties(ssplit PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(ssplit PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

if (SSPLIT_STANDALONE_COMPILE)
  include_directories(3rd-party/CLI11)
  add_executable(ssplit_executable command/ssplit_main.cpp)
  target_link_libraries(ssplit_executable ssplit ${EXT_LIBS} ${ABSEIL_LIBS})
  set_target_properties(ssplit_executable PROPERTIES OUTPUT_NAME
    "${CMAKE_BINARY_DIR}/ssplit")
endif(SSPLIT_STANDALONE_COMPILE)
