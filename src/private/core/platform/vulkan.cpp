/*
 * MIT License
 *
 * Copyright (c) 2023 Albin Johansson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "glow/core/platform/vulkan.hpp"

#include <iostream>  // cerr

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "glow/core/foundation/error.hpp"

namespace glow {

auto get_vulkan_instance_extensions(Window& window) -> std::vector<const char*>
{
  if (window.get_graphics_api() != GraphicsAPI::Vulkan) {
    throw Error {"Cannot call get_vulkan_instance_extensions on non-Vulkan window"};
  }

  auto* handle = static_cast<SDL_Window*>(window.get_handle());

  uint extension_count = 0;
  SDL_Vulkan_GetInstanceExtensions(handle, &extension_count, nullptr);

  std::vector<const char*> extensions;
  extensions.resize(static_cast<usize>(extension_count));
  SDL_Vulkan_GetInstanceExtensions(handle, &extension_count, extensions.data());

  return extensions;
}

auto make_vulkan_surface(Window& window, VkInstance instance) -> VkSurfaceKHR
{
  if (window.get_graphics_api() != GraphicsAPI::Vulkan) {
    throw Error {"Cannot call make_vulkan_surface on non-Vulkan window"};
  }

  auto* handle = static_cast<SDL_Window*>(window.get_handle());

  VkSurfaceKHR surface = nullptr;
  SDL_Vulkan_CreateSurface(handle, instance, &surface);

  if (!surface) {
    std::cerr << "Could not create Vulkan surface: " << SDL_GetError() << '\n';
    throw Error {"Could not create Vulkan surface"};
  }

  return surface;
}

}  // namespace glow
