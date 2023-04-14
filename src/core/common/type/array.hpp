#pragma once

#include <array>  // array

#include "common/primitives.hpp"

namespace glow {

template <typename T, usize N>
using Array = std::array<T, N>;

}  // namespace glow
