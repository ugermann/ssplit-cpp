#pragma once
#include <map>
#include <string>

// We need the extra level of macro redirection to
// make the preprocessor to expand the macro
#define CONVERT_TO_STRING_(x) #x
#define CONVERT_TO_STRING(x) CONVERT_TO_STRING_(x)

// #pragma message(CONVERT_TO_STRING(__cplusplus))
#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 6
// use std::experimental::string_view on older versions of gcc
// clang exports GNUC v 4.2
#  include <experimental/string_view>
#elif defined(__clang__) && __cplusplus < 201703L
// Older versions of cmake don't support CMAKE_CXX_STANDARD 17.
// On Ubuntu, the corresponding slang version finds string_view here:
#  include <experimental/string_view>
#else
#  include <string_view>
#endif

namespace ug {
namespace ssplit {
#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 6
typedef std::experimental::string_view string_view;
#elif defined(__clang__) && __cplusplus < 201703L
typedef std::experimental::string_view string_view;
#else
typedef std::string_view string_view;
#endif
}}

// #if __GNUC__ < 6 // clang exports GNUC v 4.2

// #if defined(__GNUC__) && !defined(__clang__)
// #ifndef USE_ABSEIL
// #if __GNUC__ < 6 // clang exports GNUC v 4.2
// #include <experimental/string_view>
// #else
// #include <string_view>
// #endif
// #endif
// #endif

// #ifdef USE_ABSEIL
// #include "absl/strings/string_view.h"
// #endif  // USE_ABSEIL

namespace ug {
namespace ssplit {

// #ifdef USE_ABSEIL
// typedef absl::string_view string_view;
// // USE_ABSEIL
// #elif defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 6
// typedef std::experimental::string_view string_view;
// #else
// typedef std::string_view string_view;
// #endif  //  USE_ABSEIL

// Before Jerin Philip asks again "What sort of mess is this?"
// This is C++14 or greater, which allows you to use specify the comparison
// function (class) for maps explicitly and thus allows you to use string_view
// directly as keys instead of converting them to std::string first.
typedef std::map<std::string, int, std::less<>> prefix_map_t;
}  // namespace ssplit
}  // namespace ug
