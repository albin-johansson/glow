#pragma once

#include <functional>     // less
#include <set>            // set
#include <unordered_set>  // unordered_set

namespace glow {

template <typename T>
using Set = std::set<T, std::less<>>;

template <typename T>
using HashSet = std::unordered_set<T>;

}  // namespace glow
