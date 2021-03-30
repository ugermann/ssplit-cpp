# Which version of string_view we use depends on the value of
# USE_ABSEIL. #cmakedefine01 is available only from cmake >= 3.10,
# but Ubuntu 16.04 ships with cmake 3.5, so we can't use that and
# have to resort to an intermediate variable.
# We generate a custom typedef file ssplit_typedefs.h that we
# pace in ${CMAKE_BINARY_DIR}/include/ssplit}
if (USE_ABSEIL)
  set(SSPLIT_USE_ABSEIL 1)
else()
  set(SSPLIT_USE_ABSEIL 0)
endif()
configure_file(ssplit/ssplit_typedefs.h.in
  ${CMAKE_BINARY_DIR}/include/ssplit/ssplit_typedefs.h)

include_directories(ssplit ${CMAKE_BINARY_DIR}/include/ssplit)
add_library(ssplit STATIC ssplit/ssplit.cpp ssplit/regex.cpp)
target_link_libraries(ssplit ${EXT_LIBS})
set_target_properties(ssplit PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

if (SSPLIT_STANDALONE_COMPILE)
  include_directories(3rd-party/CLI11)
  add_executable(ssplit_executable command/ssplit_main.cpp)
  target_link_libraries(ssplit_executable ssplit)
  set_target_properties(ssplit_executable PROPERTIES OUTPUT_NAME
    "${CMAKE_BINARY_DIR}/ssplit")
endif(SSPLIT_STANDALONE_COMPILE)
