#pragma once

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "common/predef.hpp"

namespace gravel::vlk {

/// RAII wrapper for `VkSurfaceKHR`.
/// The Vulkan instance must have been initialized before a surface may be created.
class Surface final {
 public:
  GRAVEL_DELETE_COPY(Surface);
  GRAVEL_DELETE_MOVE(Surface);

  explicit Surface(SDL_Window* window);

  ~Surface() noexcept;

  [[nodiscard]] auto get() -> VkSurfaceKHR { return mSurface; }

 private:
  VkSurfaceKHR mSurface {VK_NULL_HANDLE};
};

}  // namespace gravel::vlk
