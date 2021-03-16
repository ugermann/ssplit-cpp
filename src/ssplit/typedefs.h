#pragma once
#include <string>
#if USE_ABSEIL
#  include "absl/strings/string_view.h"
#  include "absl/container/flat_hash_map.h"
#else
#  include <map>
#endif

namespace ug{
namespace ssplit{

#if USE_ABSEIL
  typedef absl::string_view string_view;
  typedef absl::flat_hash_map<std::string, int> prefix_map_t;
#else
  typedef std::string_view string_view;
  typedef std::map<std::string, int, std::less<> > prefix_map_t;
#endif
}}
