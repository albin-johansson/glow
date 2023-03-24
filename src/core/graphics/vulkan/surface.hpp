#pragma once

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "common/predef.hpp"

namespace gravel::vk {

/// RAII wrapper for `VkSurfaceKHR`.
class Surface final {
 public:
  GRAVEL_DELETE_COPY(Surface);
  GRAVEL_DELETE_MOVE(Surface);

  Surface(SDL_Window* window, VkInstance instance);

  ~Surface();

  [[nodiscard]] auto get() -> VkSurfaceKHR { return mSurface; }

 private:
  VkInstance mInstance {};
  VkSurfaceKHR mSurface {};
};

}  // namespace gravel::vk
