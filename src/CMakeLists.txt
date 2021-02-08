add_subdirectory(3rd-party/abseil-cpp)
include_directories(3rd-party/abseil-cpp)
list(APPEND ABSEILLIBS
  absl::base
  absl::container
  absl::strings
  absl::hash
  absl::container_memory
  absl::hash_function_defaults
  absl::raw_hash_map
  absl::algorithm_container
  absl::memory)

include_directories(ssplit)
add_library(ssplit STATIC ssplit/ssplit.cpp ssplit/regex.cpp)
target_link_libraries(ssplit ${EXT_LIBS} ${ABSEILLIBS})
set_target_properties(ssplit PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(ssplit PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

set_target_properties(ssplit PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(ssplit PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")


if (SSPLIT_STANDALONE_COMPILE)
  include_directories(3rd-party/CLI11)
  add_executable(ssplit.exe command/test.cpp)
  add_executable(test2.exe command/test2.cpp)
  add_executable(test3.exe command/test3.cpp)
  target_link_libraries(ssplit.exe ssplit ${ABSEILLIBS})
  target_link_libraries(test2.exe ssplit ${ABSEILLIBS})
  target_link_libraries(test3.exe ssplit ${ABSEILLIBS})
  set_target_properties(ssplit PROPERTIES OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  set_target_properties(ssplit.exe PROPERTIES OUTPUT_NAME "${CMAKE_BINARY_DIR}/ssplit")
  set_target_properties(test2.exe PROPERTIES OUTPUT_NAME "${CMAKE_BINARY_DIR}/test2")
  set_target_properties(test3.exe PROPERTIES OUTPUT_NAME "${CMAKE_BINARY_DIR}/test3")
endif(SSPLIT_STANDALONE_COMPILE)
