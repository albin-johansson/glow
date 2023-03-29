#include "surface.hpp"

#include <SDL2/SDL_vulkan.h>
#include <fmt/format.h>

#include "common/debug/error.hpp"
#include "graphics/vulkan/context.hpp"

namespace gravel::vlk {

Surface::Surface(SDL_Window* window, VkInstance instance)
    : mInstance {instance}
{
  if (!SDL_Vulkan_CreateSurface(window, mInstance, &mSurface)) {
    throw Error {fmt::format("[VK] Could not create Vulkan surface: {}", SDL_GetError())};
  }

  set_surface(mSurface);
}

Surface::~Surface()
{
  vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
}

}  // namespace gravel::vlk
