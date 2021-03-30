cmake_minimum_required (VERSION 3.5.1)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_BUILD_TYPE "Release")

project (ssplit CXX)
# ssplit-cpp depends on the pcre2 library and some version of string_view
# that is compatible with std::string_view in c++17.
# If cmake and the compiler support c++17, we use std::string_view.
# If not, we download and compile the abseil libraries and use those.
# If pcre2 is available system-wide (including header files), we use that.
# if not, we download and compile.
# By default, either of the two 3rd-party resources are custom-built only
# when necessary. The two options below allow us to force their use for
# testing purposes.
option(USE_ABSEIL "Force use of the abseil library (works with C++11)." OFF)
option(USE_INTERNAL_PCRE2 "Force use internal PCRE2 instead of system PCRE2" OFF)

# Does the current version of cmake supports C++17?
list(FIND CMAKE_CXX_COMPILE_FEATURES cxx_std_17 CMAKE_SUPPORTS_CXX17)
if (${CMAKE_SUPPORTS_CXX17} EQUAL -1) # No, it doesn't.
  message(WARNING "C++-17 not supported by current setup. Forcing use of abseil.")
  # C++-17 is not available
  set(USE_ABSEIL TRUE CACHE BOOL "Force abseil" FORCE)
  set(CMAKE_CXX_STANDARD 11)
else()                                # Yes, it does.
  set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -O3")
if (USE_ABSEIL)
  message("COMPILING WITH ABSEIL ${USE_ABSEIL}")
  include(ProvideAbseil)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_ABSEIL=1")
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
set(CMAKE_FIND_LIBRARY_SUFFIXES .a)

# Make sure we have pcre2 (through system or custom download and build).
# Seee cmake/ProvidePCRE2.cmake.
include(ProvidePCRE2)
if(PCRE2_FOUND)
  include_directories(${PCRE2_INCLUDE_DIRS})
  set(EXT_LIBS ${EXT_LIBS} ${PCRE2_LIBRARIES} ${ABSEIL_LIBS})
else(PCRE2_FOUND)
  message(FATAL_ERROR "Cannot find pcrecpp libraries. Terminating.")
endif(PCRE2_FOUND)
message("   Using these libraries:")
foreach(lib ${EXT_LIBS})
  message("    - ${lib}")
endforeach()


add_subdirectory(src)

if(USE_INTERNAL_PCRE2)
  add_dependencies(ssplit pcre2) # this needs to happen after adding src
endif(USE_INTERNAL_PCRE2)

if(USE_ABSEIL)
  # target_compile_options(ssplit PUBLIC -DUSE_ABSEIL=1)
  add_dependencies(ssplit abseil) # this needs to happen after adding src
endif(USE_ABSEIL)
