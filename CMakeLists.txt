# based on https://cmake.org/cmake-tutorial/
# and CMakeLists.txt from github.com/marian-nmt/marian-dev
cmake_minimum_required (VERSION 3.5.1)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_BUILD_TYPE "Release")

project (ssplit CXX)

# Default is C++17, check if compiler supports, else hard-fail asking user to explicitly mention C++-11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
if(NOT COMPILER_SUPPORTS_CXX17 AND NOT USE_CXX11)
    message(SEND_ERROR "Please enable -DUSE_CXX11=on as compiler does not support C++17." 
                       "This will enable abseil-cpp library")
endif()


option(SSPLIT_USE_CXX11 "Restrict to C++-11, enabes abseil string-view" OFF)
option(SSPLIT_USE_INTERNAL_PCRE2 "Use internal PCRE2 instead of system PCRE2" OFF)
option(SSPLIT_COMPILE_LIBRARY_ONLY "Compile library only" OFF)

# Make SSPLIT_ prefixed option local.
set(USE_CXX_11 ${SSPLIT_USE_CXX11})
set(USE_INTERNAL_PCRE2 ${SSPLIT_USE_INTERNAL_PCRE2})
set(COMPILE_LIBRARY_ONLY ${SSPLIT_COMPILE_LIBRARY_ONLY})

# Handle C++ Standards
if (USE_CXX11)
  set(CMAKE_CXX_STANDARD 11)

  # If CXX11, abseil-cpp has to be enabled and added to EXT_LIBS, since there's
  # no way to obtain string_view (required) without.
  set(USE_ABSEIL ON)
  include(ProvideAbseil)
  set(EXT_LIBS ${EXT_LIBS} ${ABSEIL_LIBS})

  # The source files are imported in the library above. If included as a
  # subdirectory, we would want this definition to stick for future includes as
  # well. Hence add_definitions here.
  add_definitions(-DUSE_ABSEIL)
else(USE_CXX11)
  set(CMAKE_CXX_STANDARD 17)
endif(USE_CXX11)

# Enforce the standard set above.
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_FIND_LIBRARY_SUFFIXES .a)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -O3")

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
