include(ExternalProject)
set(ABSEIL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/3rd-party/abseil-cpp")

if (EXISTS ${ABSEIL_SOURCE_DIR}/.git)
  # no need to clone again
  set(ABSEIL_GIT_REPO "")
else()
  set(ABSEIL_GIT_REPO "https://github.com/abseil/abseil-cpp.git")
endif()
message("ABSEIL_GIT_REPO is ${ABSEIL_GIT_REPO}")

if(CMAKE_CXX_COMPILER MATCHES "/em\\+\\+(-[a-zA-Z0-9.])?$")
  set(ABSEIL_CMAKE_ARGS_FOR_WASM "-msimd128 -Wno-deprecated-copy-dtor")
endif()

ExternalProject_Add(abseil
  PREFIX ${CMAKE_BINARY_DIR}/abseil
  GIT_REPOSITORY ${ABSEIL_GIT_REPO}
  GIT_SHALLOW ON
  GIT_PROGRESS OFF
  SOURCE_DIR ${ABSEIL_SOURCE_DIR}
  INSTALL_DIR ${CMAKE_BINARY_DIR}
  CMAKE_ARGS
  "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}"
  "-DCMAKE_CXX_FLAGS='${ABSEIL_CMAKE_ARGS_FOR_WASM}'")

include_directories(${CMAKE_BINARY_DIR}/include)
foreach(alib base throw_delegate city strings hash raw_hash_set)
  string(CONCAT alibfull
    "${CMAKE_BINARY_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}"
    "absl_${alib}${CMAKE_STATIC_LIBRARY_SUFFIX}")
  set(ABSEIL_LIBS ${ABSEIL_LIBS} ${alibfull})
endforeach()
