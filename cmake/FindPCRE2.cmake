# Depending on the value of USE_INTERNAL_PRCRE2 this cmake file
# either tries to find the Perl Compatible Regular Expresison library (pcre2)
# on the system (when OFF), or downloads and compiles them locally (when ON).

# The following variables are set:
# PCRE2_FOUND - System has the PCRE library
# PCRE2_LIBRARIES - The PCRE library file
# PCRE2_INCLUDE_DIRS - The folder with the PCRE headers

if(USE_INTERNAL_PCRE2)
  include(ExternalProject)

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
    message("Downloading pcre2 source code from ${PCRE2_URL}")
  endif()

  # Set configure options for internal pcre2 depeding on compiler
  if(CMAKE_CXX_COMPILER MATCHES "/em\\+\\+(-[a-zA-Z0-9.])?$")
    # jit compilation isn't supported by wasm
    set(PCRE2_JIT_OPTION  "-DPCRE2_SUPPORT_JIT=OFF")
  else()
    set(PCRE2_JIT_OPTION  "-DPCRE2_SUPPORT_JIT=ON")
  endif()
  set(PCRE2_CONFIGURE_OPTIONS
    -DBUILD_SHARED_LIBS=OFF
    -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
    -DCMAKE_BUILD_TYPE=Release
    ${PCRE2_JIT_OPTION}
    )

  # set include dirs and libraries for PCRE2
  set(PCRE2_LIBRARIES ${CMAKE_BINARY_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}pcre2-8${CMAKE_STATIC_LIBRARY_SUFFIX})
  set(PCRE2_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/include")
  set(PCRE2_FOUND TRUE CACHE BOOL "Found PCRE2 libraries" FORCE)
  
  # download, configure, compile
  ExternalProject_Add(pcre2
    PREFIX ${CMAKE_BINARY_DIR}/pcre2
    URL ${PCRE2_URL}
    DOWNLOAD_DIR ${PCRE2_SRC_DIR}
    SOURCE_DIR ${PCRE2_SRC_DIR}
    CONFIGURE_COMMAND ${CMAKE_COMMAND} ${PCRE2_SRC_DIR} ${PCRE2_CONFIGURE_OPTIONS}
    BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR>
    INSTALL_DIR ${CMAKE_BINARY_DIR}
    BUILD_BYPRODUCTS ${PCRE2_LIBRARIES})

else(USE_INTERNAL_PCRE2)
  
  find_library(PCRE2_LIBRARIES NAMES pcre2 pcre2-8)
  find_path(PCRE2_INCLUDE_DIRS pcre2.h)

endif(USE_INTERNAL_PCRE2)

if(PCRE2_LIBRARIES AND PCRE2_INCLUDE_DIRS)
  # message(STATUS "PCRE2 libs: ${PCRE2_LIBRARIES}")
  # message(STATUS "PCRE2 include directory: ${PCRE2_INCLUDE_DIRS}")
  set(PCRE2_FOUND TRUE CACHE BOOL "Found PCRE2 libraries" FORCE)
  # add_custom_target(pcre2)
else()
  set(PCRE2_FOUND FALSE CACHE BOOL "Found PCRE2 libraries" FORCE)
  # message(STATUS "PCRE2 library not found.")
endif()
