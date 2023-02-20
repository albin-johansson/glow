#pragma once

#include <memory>  // unique_ptr

#include <SDL2/SDL.h>

#include "graphics/graphics_api.hpp"

namespace gravel {

struct WindowDeleter final {
  void operator()(SDL_Window* window) noexcept;
};

/// Simple RAII wrapper around an SDL window instance.
class Window final {
 public:
  explicit Window(GraphicsApi api);

  [[nodiscard]] auto get_handle() -> SDL_Window* { return mWindow.get(); }

 private:
  std::unique_ptr<SDL_Window, WindowDeleter> mWindow {};
};

}  // namespace gravel
