# based on https://cmake.org/cmake-tutorial/
# and CMakeLists.txt from github.com/marian-nmt/marian-dev
cmake_minimum_required (VERSION 3.5.1)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_BUILD_TYPE "Release")

project (ssplit CXX)

option(USE_INTERNAL_PCRE2 "Use internal PCRE2 instead of system PCRE2" OFF)

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

if(USE_INTERNAL_PCRE2)
  include(ExternalProject)
  # todo: check if wasm compile and do not enable-jit
  set(PCRE2_VERSION "10.36")
  set(PCRE2_TARBALL "pcre2-${PCRE2_VERSION}.tar.gz")
  set(PCRE2_SRC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/src/3rd-party/pcre2-${PCRE2_VERSION})

  # Download tarball only if we don't have the pcre2 source code yet.
  # For the time being, we download and unpack pcre2 into
  # the ssplit source tree. This is not particularly clean
  # but allows us to wipe the build dir without having to
  # re-download pcre2 so often. Git has been instructed to ignore
  # ${PCRE2_SRC_DIR} via .gitignore.
  if (EXISTS ${PCRE2_SRC_DIR}/configure)
    set(PCRE2_URL "")
  else()
    set(PCRE2_URL "https://ftp.pcre.org/pub/pcre/${PCRE2_TARBALL}")
  endif()

  # Set configure options for internal pcre2 depeding on compiler
  if(CMAKE_CXX_COMPILER MATCHES "/em\\+\\+(-[a-zA-Z0-9.])?$")
    # jit compilation isn't supported by wasm
    set(PCRE2_JIT_OPTION  "--disable-jit")
  else()
    set(PCRE2_JIT_OPTION  "--enable-jit")
  endif()
  set(PCRE2_CONFIGURE_OPTIONS
    --disable-shared
    --prefix=${CMAKE_CURRENT_BINARY_DIR}
    ${PCRE2_JIT_OPTION}
    )

  message("Downloading pcre2 source code from ${PCRE2_URL}")

  ExternalProject_Add(libpcre2
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/pcre2
    URL ${PCRE2_URL}
    DOWNLOAD_DIR ${PCRE2_SRC_DIR}
    SOURCE_DIR ${PCRE2_SRC_DIR}
    CONFIGURE_COMMAND ${PCRE2_SRC_DIR}/configure ${PCRE2_CONFIGURE_OPTIONS}
    INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}
    )

  # set include dirs and libraries for PCRE2
  set(PCRE2_INCLUDE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/include")
  set(PCRE2_LIBRARIES ${CMAKE_CURRENT_BINARY_DIR}/lib/libpcre2-8.a)
  set(PCRE2_FOUND TRUE CACHE BOOL "Found PCRE2 libraries" FORCE)
else(USE_INTERNAL_PCRE2)
  find_package(PCRE2)
endif(USE_INTERNAL_PCRE2)

if(PCRE2_FOUND)
  include_directories(${PCRE2_INCLUDE_DIRS})
  set(EXT_LIBS ${EXT_LIBS} ${PCRE2_LIBRARIES})
  message("   Using these libraries: ${EXT_LIBS}")
else(PCRE2_FOUND)
  message(SEND_ERROR "Cannot find pcrecpp libraries. Terminating.")
endif(PCRE2_FOUND)

add_subdirectory(src)
if(USE_INTERNAL_PCRE2)
  add_dependencies(ssplit libpcre2) # this needs to happen after adding src
endif(USE_INTERNAL_PCRE2)
