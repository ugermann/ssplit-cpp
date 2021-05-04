#pragma once
#include <map>
#include <string>

// We need the extra level of macro redirection below to make the
// preprocessor to expand the macro CONVERT_TO_STRING as intended.
// Uncomment the code block below if you need to debug C++ version
// settings.
//
// #define CONVERT_TO_STRING_(x) #x
// #define CONVERT_TO_STRING(x) CONVERT_TO_STRING_(x)
// #pragma message(CONVERT_TO_STRING(__cplusplus))


#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 6
// On gcc version 5.x , string_view is povided as
// std::experimental::string_view
// Newer versions of clang apparently export GNUC v 4.2, so we need
// the extra check to exclude the __clang__ scenario.
#include <experimental/string_view>
#elif defined(__clang__) && __cplusplus < 201703L
// Older versions of cmake (e.g. 3.8 on Ubuntu 16.04) don't support
// CMAKE_CXX_STANDARD 17. On Ubuntu 16.04, clang version also
// finds string_view here:
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

// Before Jerin Philip asks again "What sort of mess is this?"
// This is C++14 or greater, which allows you to use specify the comparison
// function (class) for maps explicitly and thus allows you to use string_view
// directly as keys instead of converting them to std::string first.
typedef std::map<std::string, int, std::less<>> prefix_map_t;
}  // namespace ssplit
}  // namespace ug
