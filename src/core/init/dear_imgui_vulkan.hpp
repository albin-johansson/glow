#pragma once

#include <SDL2/SDL.h>

#include "common/predef.hpp"

namespace glow {

class DearImGuiVulkan final {
 public:
  GLOW_DELETE_COPY(DearImGuiVulkan);
  GLOW_DELETE_MOVE(DearImGuiVulkan);

  explicit DearImGuiVulkan(SDL_Window* window);

  ~DearImGuiVulkan();

  static void recreate_font_textures();
};

}  // namespace glow
