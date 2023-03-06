#pragma once

#include <functional>  // hash

#include "common/primitives.hpp"

namespace gravel {

template <typename T>
void hash_combine(usize& seed, const T& val)
{
  // https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0814r0.pdf
  seed ^= std::hash<T> {}(val) + 0x9E3779B9 + (seed << 6) + (seed >> 2);
}

[[nodiscard]] auto hash_combine(const auto&... args) -> usize
{
  usize seed = 0;
  (hash_combine(seed, args), ...);
  return seed;
}

}  // namespace gravel