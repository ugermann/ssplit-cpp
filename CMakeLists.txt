# based on https://cmake.org/cmake-tutorial/
# and CMakeLists.txt from github.com/marian-nmt/marian-dev
cmake_minimum_required (VERSION 3.5.1)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_BUILD_TYPE "Release")

project (ssplit CXX)

# Default is C++17, check if compiler supports, else hard-fail
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
if(NOT COMPILER_SUPPORTS_CXX17 AND NOT USE_CXX11)
   message(SEND_ERROR "Please enable -DUSE_CXX11=on as compiler does not support C++17")
endif()


option(USE_CXX11 "Restrict to c++-11" OFF)
option(USE_INTERNAL_PCRE2 "Use internal PCRE2 instead of system PCRE2" OFF)
option(USE_ABSEIL "Use the abseil library (works with C++11)." OFF)
option(COMPILE_LIBRARY_ONLY "Compile library only" OFF)

# Handle CMake Standards
if (USE_CXX11)
  set(CMAKE_CXX_STANDARD 11)
  set(USE_ABSEIL ON)
else(USE_CXX11)
  set(CMAKE_CXX_STANDARD 17)
endif(USE_CXX11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_FIND_LIBRARY_SUFFIXES .a)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -O3")

if (USE_ABSEIL)
  include(ProvideAbseil)
  add_definitions(-DUSE_ABSEIL)
  set(EXT_LIBS ${EXT_LIBS} ${ABSEIL_LIBS})
endif(USE_ABSEIL)

include(FindPCRE2)
if(PCRE2_FOUND)
  include_directories(${PCRE2_INCLUDE_DIRS})
  set(EXT_LIBS ${EXT_LIBS} ${PCRE2_LIBRARIES})
  message("Using these libraries: ${EXT_LIBS}")
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
