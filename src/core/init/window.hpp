#pragma once

#include <SDL2/SDL.h>

#include "common/type/memory.hpp"
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
  Unique<SDL_Window, WindowDeleter> mWindow;
};

}  // namespace gravel
