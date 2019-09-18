include_directories(ssplit)
include_directories(3rd-party/CLI11)

add_library(ssplit STATIC ssplit/ssplit.cpp)
set_target_properties(ssplit PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(ssplit PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_executable(ssplit.exe command/test.cpp)
target_link_libraries(ssplit.exe ssplit pcrecpp pcre)
set_target_properties(ssplit PROPERTIES OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(ssplit.exe PROPERTIES OUTPUT_NAME "${CMAKE_BINARY_DIR}/ssplit")
