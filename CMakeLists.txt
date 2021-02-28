# based on https://cmake.org/cmake-tutorial/
# and CMakeLists.txt from github.com/marian-nmt/marian-dev
cmake_minimum_required (VERSION 3.5.1)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_BUILD_TYPE "Release")

project (ssplit CXX)

option(USE_INTERNAL_PCRE2 "Use internal PCRE2 instead of system PCRE2" ON)

# Check if this is a standalone compile (=>compile executables)
# or an embedded compile (this directory was included via subdir)
get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
  set(SSPLIT_STANDALONE_COMPILE off)
else(hasParent)
  set(SSPLIT_STANDALONE_COMPILE on)
endif(hasParent)

set(CMAKE_CXX_FLAGS "-fPIC -O3")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_FIND_LIBRARY_SUFFIXES .a)

include(FindPCRE2)

if(PCRE2_FOUND)
  include_directories(${PCRE2_INCLUDE_DIRS})
  set(EXT_LIBS ${EXT_LIBS} ${PCRE2_LIBRARIES})
  message("   Using these libraries: ${EXT_LIBS}")
else(PCRE2_FOUND)
  message(SEND_ERROR "Cannot find pcrecpp libraries. Terminating.")
endif(PCRE2_FOUND)

add_subdirectory(src)
if(USE_INTERNAL_PCRE2)
  add_dependencies(ssplit pcre2) # this needs to happen after adding src
endif(USE_INTERNAL_PCRE2)
