#pragma once

#include <SDL2/SDL.h>

#include "common/predef.hpp"
#include "common/type/maybe.hpp"
#include "graphics/graphics_api.hpp"
#include "init/dear_imgui_opengl.hpp"
#include "init/dear_imgui_vulkan.hpp"

namespace glow {

class DearImGui final {
  inline static constexpr float kMinFontSize = 9;
  inline static constexpr float kDefFontSize = 13;
  inline static constexpr float kMaxFontSize = 37;

 public:
  GRAVEL_DELETE_COPY(DearImGui);
  GRAVEL_DELETE_MOVE(DearImGui);

  DearImGui(SDL_Window* window, GraphicsApi api);

  ~DearImGui();

  void reload_fonts();

  void increase_font_size();

  void decrease_font_size();

  void reset_font_size();

 private:
  Maybe<DearImGuiOpenGL> mGL;
  Maybe<DearImGuiVulkan> mVK;
  float mFontSize {kDefFontSize};
};

}  // namespace glow
