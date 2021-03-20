#pragma once
#include <string>
#ifdef USE_ABSEIL
#include "absl/container/flat_hash_map.h"
#include "absl/strings/string_view.h"
#else  // USE_ABSEIL
#include <map>
#endif  // USE_ABSEIL

namespace ug {
namespace ssplit {

#ifdef USE_ABSEIL
typedef absl::string_view string_view;
typedef absl::flat_hash_map<std::string, int> prefix_map_t;
#else   // USE_ABSEIL
typedef std::string_view string_view;
typedef std::map<std::string, int, std::less<> > prefix_map_t;
#endif  //  USE_ABSEIL
}  // namespace ssplit
}  // namespace ug
