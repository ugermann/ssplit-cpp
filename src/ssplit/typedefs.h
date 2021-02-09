#pragma once
// #include "absl/strings/string_view.h"
// #include "absl/container/flat_hash_map.h"
#include <string>
#include <unordered_map>
#include <map>

namespace ug{
namespace ssplit{

// typedef absl::string_view string_view;
// typedef absl::flat_hash_map<std::string, int> prefix_map_t;

typedef std::string_view string_view;
typedef std::map<std::string, int, std::less<> > prefix_map_t;

}}
