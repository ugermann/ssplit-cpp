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

set(CMAKE_CXX_FLAGS "-fPIC -O3")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(USE_INTERNAL_PCRE2 "Uses internal PCRE2 instead of system PCRE2" OFF)

if(USE_INTERNAL_PCRE2)
   set(PCRE2_VERSION "10.00")
   set(PCRE2_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/3rd-party/pcre2")

   if(NOT EXISTS ${PCRE2_SOURCE_DIR})
       message("Downloading pcre2-${PCRE2_VERSION} into ${PCRE2_SOURCE_DIR}")
       set(PCRE2_DOWNLOAD_URL "https://ftp.pcre.org/pub/pcre/pcre2-${PCRE2_VERSION}.tar.bz2")
       file(DOWNLOAD
           ${PCRE2_DOWNLOAD_URL} # URL
           "${CMAKE_CURRENT_BINARY_DIR}/pcre2.tar.bz2" # FILE
        )
    file(ARCHIVE_EXTRACT INPUT "${CMAKE_CURRENT_BINARY_DIR}/pcre2.tar.bz2" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(RENAME "${CMAKE_CURRENT_BINARY_DIR}/pcre2-${PCRE2_VERSION}" "${PCRE2_SOURCE_DIR}")
   endif()

   add_subdirectory(${PCRE2_SOURCE_DIR})
   set(EXT_LIBS ${EXT_LIBS} pcre2-8)
   include_directories(${CMAKE_CURRENT_BINARY_DIR}/src/3rd-party/pcre2)
else(USE_INTERNAL_PCRE2)
    find_package(PCRE2)
    if(PCRE2_FOUND)
      include_directories(${PCRE2_INCLUDE_DIRS})
      set(EXT_LIBS ${EXT_LIBS} ${PCRE2_LIBRARIES})
      message("Using system-wide libpcre2: ${PCRE2_LIBRARIES}")
    else(PCRE2_FOUND)
      message(SEND_ERROR "Cannot find pcrecpp libraries. Terminating.")
    endif(PCRE2_FOUND)
endif(USE_INTERNAL_PCRE2)

add_subdirectory(src)

