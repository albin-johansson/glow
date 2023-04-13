#pragma once

#include <SDL2/SDL.h>

#include "common/predef.hpp"

namespace gravel {

class DearImGuiVulkan final {
 public:
  GRAVEL_DELETE_COPY(DearImGuiVulkan);
  GRAVEL_DELETE_MOVE(DearImGuiVulkan);

  explicit DearImGuiVulkan(SDL_Window* window);

  ~DearImGuiVulkan();

  static void recreate_font_textures();
};

}  // namespace gravel
