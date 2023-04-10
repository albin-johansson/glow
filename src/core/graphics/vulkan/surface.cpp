#include "surface.hpp"

#include <SDL2/SDL_vulkan.h>
#include <fmt/format.h>

#include "common/debug/assert.hpp"
#include "common/debug/error.hpp"
#include "graphics/vulkan/context.hpp"
#include "init/window.hpp"

namespace gravel::vlk {

void SurfaceDeleter::operator()(VkSurfaceKHR surface) noexcept
{
  vkDestroySurfaceKHR(get_instance(), surface, nullptr);
}

auto create_surface() -> Surface
{
  GRAVEL_ASSERT(get_window() != nullptr);
  GRAVEL_ASSERT(get_instance() != VK_NULL_HANDLE);

  VkSurfaceKHR surface = VK_NULL_HANDLE;
  if (!SDL_Vulkan_CreateSurface(get_window(), get_instance(), &surface)) {
    throw Error {fmt::format("[VK] Could not create Vulkan surface: {}", SDL_GetError())};
  }

  set_surface(surface);
  return Surface {surface};
}

}  // namespace gravel::vlk
