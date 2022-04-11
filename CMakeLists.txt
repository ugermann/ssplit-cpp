# based on https://cmake.org/cmake-tutorial/
# and CMakeLists.txt from github.com/marian-nmt/marian-dev
cmake_minimum_required (VERSION 3.5.1)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_BUILD_TYPE "Release")

project (ssplit CXX)

option(SSPLIT_USE_INTERNAL_PCRE2 "Use internal PCRE2 instead of system's." OFF)
option(SSPLIT_COMPILE_LIBRARY_ONLY "Compile library only." OFF)
option(SSPLIT_PREFER_STATIC_COMPILE "Prefer static over dynamic linking." ON)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (${CMAKE_VERSION} VERSION_GREATER "3.8.0")
  set(CMAKE_CXX_STANDARD 17)
else(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
  # Older cmake doesn't know how to set C++17 standard.
  set(CMAKE_CXX_STANDARD 14)
endif(${CMAKE_VERSION} VERSION_GREATER "3.8.0")

if(WIN32) # Windows static extension is .lib
    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2")
    else(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -O3")
    endif(MSVC)
else(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -O3")
endif(WIN32)

IF(SSPLIT_PREFER_STATIC_COMPILE)
  IF(WIN32)
      SET(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
  ELSE(WIN32)
      SET(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
  ENDIF(WIN32)
ENDIF(SSPLIT_PREFER_STATIC_COMPILE)

include(GNUInstallDirs)
include(FindPCRE2)
if(PCRE2_FOUND)
  include_directories(${PCRE2_INCLUDE_DIRS})
  set(EXT_LIBS ${EXT_LIBS} ${PCRE2_LIBRARIES})
  message("   Using these libraries: ${EXT_LIBS}")
else(PCRE2_FOUND)
  message(SEND_ERROR "Cannot find pcre2 libraries. Terminating.")
endif(PCRE2_FOUND)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
message("Added subdirectory ${CMAKE_CURRENT_SOURCE_DIR}/src")
if(SSPLIT_USE_INTERNAL_PCRE2)
  add_dependencies(ssplit pcre2) # this needs to happen after adding src
endif(SSPLIT_USE_INTERNAL_PCRE2)
