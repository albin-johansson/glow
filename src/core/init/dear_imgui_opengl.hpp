#pragma once

#include <SDL2/SDL.h>

#include "common/predef.hpp"

namespace glow {

class DearImGuiOpenGL final {
 public:
  GLOW_DELETE_COPY(DearImGuiOpenGL);
  GLOW_DELETE_MOVE(DearImGuiOpenGL);

  explicit DearImGuiOpenGL(SDL_Window* window);

  ~DearImGuiOpenGL();

  void recreate_font_textures();
};

}  // namespace glow
