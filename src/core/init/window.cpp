#include "window.hpp"

#include <fmt/format.h>

#include "common/debug/error.hpp"
#include "common/primitives.hpp"

namespace gravel {
namespace {

[[nodiscard]] auto to_window_flags(const GraphicsApi api) -> uint32
{
  const auto base_flags = SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_HIDDEN;
  switch (api) {
    case GraphicsApi::OpenGL:
      return base_flags | SDL_WINDOW_OPENGL;

    default:
      throw Error {"Invalid graphics API"};
  }
}

}  // namespace

void WindowDeleter::operator()(SDL_Window* window) noexcept
{
  SDL_DestroyWindow(window);
}

Window::Window(const GraphicsApi api)
    : mWindow {SDL_CreateWindow("Gravel",
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                1000,
                                750,
                                to_window_flags(api))}
{
  if (!mWindow) {
    throw Error {fmt::format("Could not create window: {}", SDL_GetError())};
  }
}

}  // namespace gravel
