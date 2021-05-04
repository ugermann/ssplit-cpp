# based on https://cmake.org/cmake-tutorial/
# and CMakeLists.txt from github.com/marian-nmt/marian-dev
cmake_minimum_required (VERSION 3.5.1)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_BUILD_TYPE "Release")

project (ssplit CXX)

option(SSPLIT_USE_INTERNAL_PCRE2 "Use internal PCRE2 instead of system's." OFF)
option(SSPLIT_COMPILE_LIBRARY_ONLY "Compile library only." OFF)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (${CMAKE_VERSION} VERSION_GREATER "3.8.0")
  set(CMAKE_CXX_STANDARD 17)
else(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
  # Older cmake doesn't know how to set C++17 standard.
  set(CMAKE_CXX_STANDARD 14)
endif(${CMAKE_VERSION} VERSION_GREATER "3.8.0")

# WASM compiles via emscripten set CMAKE_TOOLCHAIN_FILE. TO avoid cmake
# warnings in other scenarios, we set this to an empty value.
set(CMAKE_TOOLCHAIN_FILE "")
set(USE_ABSEIL OFF) # Default USE_ABSEIL explicitly turned OFF

# Make SSPLIT_ prefixed option local.
# set(USE_INTERNAL_PCRE2 ${SSPLIT_USE_INTERNAL_PCRE2})
# set(COMPILE_LIBRARY_ONLY ${SSPLIT_COMPILE_LIBRARY_ONLY})
# set(USE_ABSEIL OFF) # Default USE_ABSEIL explicitly turned OFF
# set(EXT_LIBS "") # Initialize EXT_LIBS with a legal value.

# # Handle C++ Standards
# if (USE_CXX11)
#   set(CMAKE_CXX_STANDARD 11)

#   # If CXX11, abseil-cpp has to be enabled and added to EXT_LIBS, since there's
#   # no way to obtain string_view (required) without.
#   set(USE_ABSEIL ON)
#   include(ProvideAbseil)
#   set(EXT_LIBS ${EXT_LIBS} ${ABSEIL_LIBS} ${CMAKE_DL_LIBS})

#   # The source files are imported in the library above. If included as a
#   # subdirectory, we would want this definition to stick for future includes as
#   # well. Hence add_definitions here.
#   add_definitions(-DUSE_ABSEIL)

#   # We may need to manually specify CXX flags here;
#   if(MSVC)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++11")
#   else(MSVC)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#   endif(MSVC)
# else(USE_CXX11)
#   if(${CMAKE_VERSION} VERSION_GREATER "3.8.0") # Older cmake doesn't know how to set C++17 standard...
#     set(CMAKE_CXX_STANDARD 17)
#   endif()
#   if(MSVC)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
#   else(MSVC)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
#   endif(MSVC)
# endif(USE_CXX11)

if(WIN32) # Windows static extension is .lib
    set(CMAKE_FIND_LIBRARY_SUFFIXES .lib)
    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2")
    else(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -O3")
    endif(MSVC)
else(WIN32)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a .so)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -O3")
endif(WIN32)

include(FindPCRE2)
if(PCRE2_FOUND)
  include_directories(${PCRE2_INCLUDE_DIRS})
  set(EXT_LIBS ${EXT_LIBS} ${PCRE2_LIBRARIES})
  message("   Using these libraries: ${EXT_LIBS}")
else(PCRE2_FOUND)
  message(SEND_ERROR "Cannot find pcre2 libraries. Terminating.")
endif(PCRE2_FOUND)

add_subdirectory(src)

if(USE_INTERNAL_PCRE2)
  add_dependencies(ssplit pcre2) # this needs to happen after adding src
endif(USE_INTERNAL_PCRE2)

# if(USE_ABSEIL)
#   add_dependencies(ssplit abseil) # this needs to happen after adding src
# endif(USE_ABSEIL)
