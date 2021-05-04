#pragma once
#include <map>
#include <string>

// We need the extra level of macro redirection to
// make the preprocessor to expand the macro
#define CONVERT_TO_STRING_(x) #x
#define CONVERT_TO_STRING(x) CONVERT_TO_STRING_(x)
// #define STR2(x) #x
// #define STR(x) STR2(x)

#pragma message(CONVERT_TO_STRING(__cplusplus))
// string_view is in experimental/ prior to C++17
#if __cplusplus <= 201703L
#include <experimental/string_view>
#else
#include <string_view>
#endif

namespace ug {
namespace ssplit {
typedef std::experimental::string_view string_view;
}}

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

typedef std::map<std::string, int, std::less<>> prefix_map_t;
}  // namespace ssplit
}  // namespace ug
