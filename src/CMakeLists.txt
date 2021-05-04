include_directories(ssplit)
add_library(ssplit STATIC ssplit/ssplit.cpp ssplit/regex.cpp)
target_link_libraries(ssplit ${EXT_LIBS})
set_target_properties(ssplit PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(ssplit PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

if (NOT COMPILE_LIBRARY_ONLY)
  include_directories(3rd-party/CLI11)
  if (NOT WIN32) # ssplit-main has some mmap() code, and include "sys/mman.h" which is only available on *nix
    add_executable(ssplit_executable command/ssplit_main.cpp)
    target_link_libraries(ssplit_executable ssplit)
    set_target_properties(ssplit_executable PROPERTIES OUTPUT_NAME ssplit)
  endif (NOT WIN32)

  add_executable(ssplit_test_executable command/test_ssplit.cpp)
  target_link_libraries(ssplit_test_executable ssplit)
  set_target_properties(ssplit_test_executable PROPERTIES OUTPUT_NAME test_ssplit)
endif(NOT COMPILE_LIBRARY_ONLY)
