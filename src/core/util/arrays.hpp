#pragma once

#include "common/primitives.hpp"
#include "common/type/array.hpp"

namespace glow {

template <typename T, usize N>
[[nodiscard]] consteval auto array_length([[maybe_unused]] const T (&array)[N]) noexcept
    -> usize
{
  return N;
}

}  // namespace glow
