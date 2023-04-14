#pragma once

#include <SDL2/SDL.h>

#include "common/predef.hpp"

namespace glow {

class DearImGuiOpenGL final {
 public:
  GRAVEL_DELETE_COPY(DearImGuiOpenGL);
  GRAVEL_DELETE_MOVE(DearImGuiOpenGL);

  explicit DearImGuiOpenGL(SDL_Window* window);

  ~DearImGuiOpenGL();

  void recreate_font_textures();
};

}  // namespace glow
