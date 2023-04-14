#pragma once

#include <vector>  // vector

#include "common/primitives.hpp"

namespace glow {

template <typename T>
using Vector = std::vector<T>;

template <typename T>
[[nodiscard]] auto byte_size(const Vector<T>& vector) noexcept -> usize
{
  return vector.size() * sizeof(T);
}

}  // namespace glow