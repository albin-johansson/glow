#pragma once

#include <functional>     // less
#include <map>            // map
#include <unordered_map>  // unordered_map

namespace gravel {

template <typename K, typename V>
using Map = std::map<K, V, std::less<>>;

template <typename K, typename V>
using HashMap = std::unordered_map<K, V>;

}  // namespace gravel