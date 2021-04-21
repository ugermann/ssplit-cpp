#pragma once
#include <map>
#include <string>

#ifdef USE_ABSEIL
#include "absl/strings/string_view.h"
#endif  // USE_ABSEIL

namespace ug {
namespace ssplit {

#ifdef USE_ABSEIL
typedef absl::string_view string_view;
#else   // USE_ABSEIL
typedef std::string_view string_view;
#endif  //  USE_ABSEIL

typedef std::map<std::string, int> prefix_map_t;
}  // namespace ssplit
}  // namespace ug
