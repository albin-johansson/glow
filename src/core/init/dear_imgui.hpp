#pragma once

#include <SDL2/SDL.h>

#include "common/predef.hpp"
#include "common/type/maybe.hpp"
#include "graphics/graphics_api.hpp"
#include "init/dear_imgui_opengl.hpp"

namespace gravel {

class DearImGui final {
 public:
  GRAVEL_DELETE_COPY(DearImGui);
  GRAVEL_DELETE_MOVE(DearImGui);

  DearImGui(SDL_Window* window, GraphicsApi api);

  ~DearImGui();

  void reload_fonts();

 private:
  Maybe<DearImGuiOpenGL> mGL;
};

}  // namespace gravel
