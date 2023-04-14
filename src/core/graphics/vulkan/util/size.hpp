#pragma once

#include "common/primitives.hpp"

namespace glow::vk {

[[nodiscard]] constexpr auto u32_size(const auto& container) noexcept(
    noexcept(container.size())) -> uint32
{
  return static_cast<uint32>(container.size());
}

}  // namespace glow::vk
