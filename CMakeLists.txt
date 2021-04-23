# based on https://cmake.org/cmake-tutorial/
# and CMakeLists.txt from github.com/marian-nmt/marian-dev
cmake_minimum_required (VERSION 3.5.1)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_BUILD_TYPE "Release")

project (ssplit CXX)
include(CheckCXXCompilerFlag)
if(MSVC) # MSVC compiler has different flags
    CHECK_CXX_COMPILER_FLAG("/std:c++17" COMPILER_SUPPORTS_CXX17)
    CHECK_CXX_COMPILER_FLAG("/std:c++11" COMPILER_SUPPORTS_CXX11)
else(MSVC)
    CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
    CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
endif(MSVC)

option(USE_INTERNAL_PCRE2 "Use internal PCRE2 instead of system PCRE2" OFF)
option(USE_ABSEIL "Use the abseil library (works with C++11)." OFF)
message("COMPILING WITH ABSEIL ${USE_ABSEIL}")

if(COMPILER_SUPPORTS_CXX17)
  # With C++17, we don't need abseil.
  message("COMPILER SUPPORTS C++17")
  set(CMAKE_CXX_STANDARD 17)
else(COMPILER_SUPPORTS_CXX17)
  set(CMAKE_CXX_STANDARD 11)
  # With C++11, abseil is necessary, because std::string_view is not supported.
  set(USE_ABSEIL ON)
endif(COMPILER_SUPPORTS_CXX17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -O3")
message("CMAKE_CXX_STANDARD is ${CMAKE_CXX_STANDARD}")

if (USE_ABSEIL)
  message("COMPILING WITH ABSEIL ${USE_ABSEIL}")
  include(ProvideAbseil)
  add_definitions(-DUSE_ABSEIL)
endif(USE_ABSEIL)

# Check if this is a standalone compile (=>compile executables)
# or an embedded compile (this directory was included via subdir)
get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
  set(SSPLIT_STANDALONE_COMPILE off)
else(hasParent)
  set(SSPLIT_STANDALONE_COMPILE on)
endif(hasParent)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(WIN32) # Windows static extension is .lib
    set(CMAKE_FIND_LIBRARY_SUFFIXES .lib)
else(WIN32)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
endif(WIN32)
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

if(USE_ABSEIL)
  add_dependencies(ssplit abseil) # this needs to happen after adding src
endif(USE_ABSEIL)
