#pragma once

#include "common/type/maybe.hpp"
#include "graphics/graphics_api.hpp"
#include "graphics/opengl/context.hpp"
#include "init/dear_imgui.hpp"
#include "init/sdl.hpp"
#include "init/window.hpp"

namespace gravel {

/// Handles the initialization of fundamental resources.
class EngineInitializer final {
 public:
  explicit EngineInitializer(GraphicsApi api);

  [[nodiscard]] auto get_window() -> Window& { return mWindow; }
  [[nodiscard]] auto get_imgui() -> DearImGui& { return mImGui.value(); }

 private:
  SDL mSDL;
  Window mWindow;
  Maybe<gl::Context> mGL;
  Maybe<DearImGui> mImGui;
};

}  // namespace gravel
