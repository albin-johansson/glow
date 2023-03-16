#include "engine_initializer.hpp"

namespace gravel {

EngineInitializer::EngineInitializer(const GraphicsApi api)
    : mSDL {api},
      mWindow {api}
{
  if (api == GraphicsApi::OpenGL) {
    mGL.emplace(mWindow.get_handle());
  }

  mImGui.emplace(mWindow.get_handle(), api);
}

}  // namespace gravel
