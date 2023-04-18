#pragma once

#include <vulkan/vulkan.h>

#include "common/type/memory.hpp"

namespace glow::vk {

struct SurfaceDeleter final {
  void operator()(VkSurfaceKHR surface) noexcept;
};

using SurfacePtr = Unique<VkSurfaceKHR_T, SurfaceDeleter>;

[[nodiscard]] auto create_surface() -> SurfacePtr;

}  // namespace glow::vk
