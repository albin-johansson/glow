#pragma once

#include <vulkan/vulkan.h>

#include "common/type/memory.hpp"

namespace gravel::vlk {

struct SurfaceDeleter final {
  void operator()(VkSurfaceKHR surface) noexcept;
};

using Surface = Unique<VkSurfaceKHR_T, SurfaceDeleter>;

[[nodiscard]] auto create_surface() -> Surface;

}  // namespace gravel::vlk
