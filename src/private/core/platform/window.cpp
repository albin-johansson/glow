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

#include "glow/core/platform/window.hpp"

#include <iostream>  // cerr

#include <SDL2/SDL.h>

#include "glow/core/foundation/error.hpp"

namespace glow {
namespace {

struct SDLWindowDeleter final {
  void operator()(SDL_Window* window) noexcept { SDL_DestroyWindow(window); }
};

using UniqueSDLWindow = std::unique_ptr<SDL_Window, SDLWindowDeleter>;

}  // namespace

struct Window::Data final {
  GraphicsAPI api {GraphicsAPI::Null};
  UniqueSDLWindow window;
};

Window::Window(const GraphicsAPI api)
    : mData {std::make_unique<Data>()}
{
  auto& data = *mData;
  data.api = api;

  uint32 flags = SDL_WINDOW_HIDDEN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE;

  if (api == GraphicsAPI::OpenGL) {
    flags |= SDL_WINDOW_OPENGL;
  }
  else if (api == GraphicsAPI::Vulkan) {
    flags |= SDL_WINDOW_VULKAN;
  }

  data.window.reset(SDL_CreateWindow("Glow",
                                     SDL_WINDOWPOS_UNDEFINED,
                                     SDL_WINDOWPOS_UNDEFINED,
                                     800,
                                     600,
                                     flags));
  if (!data.window) {
    std::cerr << "Could not create window: " << SDL_GetError() << '\n';
    throw Error {"Could not create window"};
  }
}

Window::~Window() noexcept = default;

Window::Window(Window&& other) noexcept = default;

auto Window::operator=(Window&& other) noexcept -> Window& = default;

void Window::show()
{
  SDL_ShowWindow(mData->window.get());
}

void Window::hide()
{
  SDL_HideWindow(mData->window.get());
}

auto Window::get_width() const -> uint32
{
  int width = 0;
  SDL_GetWindowSizeInPixels(mData->window.get(), &width, nullptr);
  return static_cast<uint32>(width);
}

auto Window::get_height() const -> uint32
{
  int height = 0;
  SDL_GetWindowSizeInPixels(mData->window.get(), nullptr, &height);
  return static_cast<uint32>(height);
}

auto Window::get_graphics_api() const -> GraphicsAPI
{
  return mData->api;
}

auto Window::get_handle() -> void*
{
  return mData->window.get();
}

}  // namespace glow
