# Depending on the value of USE_INTERNAL_PRCRE2 this cmake file
# either tries to find the Perl Compatible Regular Expresison library (pcre2)
# on the system (when OFF), or downloads and compiles them locally (when ON).

# The following variables are set:
# PCRE2_FOUND - System has the PCRE library
# PCRE2_LIBRARIES - The PCRE library file
# PCRE2_INCLUDE_DIRS - The folder with the PCRE headers

# USE_INTERNAL_PCRE2 should be off by default and only set explicitly to test
# compilation with internal pcre2 on a system where pcre2 is actually
# available.
if(NOT USE_INTERNAL_PCRE2)
  # By default we check if pcre2 is installed on the system
  find_library(PCRE2_LIBRARIES NAMES pcre2 pcre2-8)
  find_path(PCRE2_INCLUDE_DIRS pcre2.h)
endif(NOT USE_INTERNAL_PCRE2)

# If not found, or we are deliberately using internal pcre2, we must
# possibly download and then compile pcre2.
#
# We download the tarball only if we don't have the pcre2 source code
# yet, which, for the time being, we install into the source tree so
# that it survives a wipe of the build directory. Git has been
# instructed to ignore ${PCRE2_SRC_DIR} via .gitignore.

if(NOT PCRE2_LIBRARIES OR NOT PCRE2_INCLUDE_DIRS)
  include(ExternalProject)
  set(PCRE2_VERSION "10.36")
  set(PCRE2_TARBALL "pcre2-${PCRE2_VERSION}.tar.gz")
  set(PCRE2_SRC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/src/3rd-party/pcre2-${PCRE2_VERSION})

  if (EXISTS ${PCRE2_SRC_DIR}/configure)
    # Don't download if we have the source code already
    set(PCRE2_URL "")
  else()
    set(PCRE2_URL "https://ftp.pcre.org/pub/pcre/${PCRE2_TARBALL}")
    message("Downloading pcre2 source code from ${PCRE2_URL}")
  endif()

  # Set configure options for internal pcre2 depeding on compiler
  if(CMAKE_CXX_COMPILER MATCHES "/em\\+\\+(-[a-zA-Z0-9.])?$")
    # Jit compilation isn't supported by wasm.
    set(PCRE2_JIT_OPTION  "--disable-jit")
  else()
    set(PCRE2_JIT_OPTION  "--enable-jit")
  endif()
  set(PCRE2_CONFIGURE_OPTIONS
    --disable-shared --prefix=${CMAKE_BINARY_DIR} ${PCRE2_JIT_OPTION})

  # set include dirs and libraries for PCRE2
  set(PCRE2_LIBRARIES
    ${CMAKE_BINARY_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}pcre2-8${CMAKE_STATIC_LIBRARY_SUFFIX})
  set(PCRE2_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/include")
  set(PCRE2_FOUND TRUE CACHE BOOL "Found PCRE2 libraries" FORCE)

  # download, configure, compile
  ExternalProject_Add(pcre2
    PREFIX ${CMAKE_BINARY_DIR}/pcre2
    URL ${PCRE2_URL}
    DOWNLOAD_DIR ${PCRE2_SRC_DIR}
    SOURCE_DIR ${PCRE2_SRC_DIR}
    CONFIGURE_COMMAND ${PCRE2_SRC_DIR}/configure ${PCRE2_CONFIGURE_OPTIONS}
    INSTALL_DIR ${CMAKE_BINARY_DIR}
    BUILD_BYPRODUCTS ${PCRE2_LIBRARIES})

  set(USE_INTERNAL_PCRE2 TRUE CACHE BOOL "Use internal pcre2." FORCE)
endif()

if(PCRE2_LIBRARIES AND PCRE2_INCLUDE_DIRS)
  set(PCRE2_FOUND TRUE CACHE BOOL "Found PCRE2 libraries" FORCE)
else()
  set(PCRE2_FOUND FALSE CACHE BOOL "Found PCRE2 libraries" FORCE)
endif()
