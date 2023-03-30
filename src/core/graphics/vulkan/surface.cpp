#include "surface.hpp"

#include <SDL2/SDL_vulkan.h>
#include <fmt/format.h>

#include "common/debug/assert.hpp"
#include "common/debug/error.hpp"
#include "graphics/vulkan/context.hpp"

namespace gravel::vlk {

Surface::Surface(SDL_Window* window)
{
  GRAVEL_ASSERT(get_instance() != VK_NULL_HANDLE);

  if (!SDL_Vulkan_CreateSurface(window, get_instance(), &mSurface)) {
    throw Error {fmt::format("[VK] Could not create Vulkan surface: {}", SDL_GetError())};
  }

  set_surface(mSurface);
}

Surface::~Surface() noexcept
{
  vkDestroySurfaceKHR(get_instance(), mSurface, nullptr);
}

}  // namespace gravel::vlk
