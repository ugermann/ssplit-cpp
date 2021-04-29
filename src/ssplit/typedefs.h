#pragma once
#include <map>
#include <string>

#if defined(__GNUC__) && !defined(__clang__)
#ifndef USE_ABSEIL
#if __GNUC__ < 6 // clang exports GNUC v 4.2
#include <experimental/string_view>
#else
#include <string_view>
#endif
#endif
#endif

#ifdef USE_ABSEIL
#include "absl/strings/string_view.h"
#endif  // USE_ABSEIL

namespace ug {
namespace ssplit {

#ifdef USE_ABSEIL
typedef absl::string_view string_view;
// USE_ABSEIL
#elif defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 6
typedef std::experimental::string_view string_view;
#else
typedef std::string_view string_view;
#endif  //  USE_ABSEIL

typedef std::map<std::string, int> prefix_map_t;
}  // namespace ssplit
}  // namespace ug

