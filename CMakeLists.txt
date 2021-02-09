# based on https://cmake.org/cmake-tutorial/
# and CMakeLists.txt from github.com/marian-nmt/marian-dev
cmake_minimum_required (VERSION 3.5.1)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_BUILD_TYPE "Release")

project (ssplit CXX)

# Check if this is a standalone compile (=>compile executables)
# or an embedded compile (this directory was included via subdir)
get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
  set(SSPLIT_STANDALONE_COMPILE off)
else(hasParent)
  set(SSPLIT_STANDALONE_COMPILE on)
endif(hasParent)

set(BUILD_ARCH x86-64)
set(CMAKE_CXX_FLAGS "-std=c++11 -fPIC -march=${BUILD_ARCH} -O3")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PCRECPP)
find_package(PCRE2)
if(PCRE2_FOUND)
  include_directories(${PCRE2_INCLUDE_DIRS} ${PCRECPP_INCLUD_DIRS})
  set(EXT_LIBS ${EXT_LIBS} ${PCRE2_LIBRARIES} ${PCRECPP_LIBRARIES})
else(PCRE2_FOUND)
  message(SEND_ERROR "Cannot find pcrecpp libraries. Terminating.")
endif(PCRE2_FOUND)

message("Using PCRECPP: ${PCRE2_LIBRARIES}")
add_subdirectory(src)
