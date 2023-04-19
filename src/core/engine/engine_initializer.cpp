#include "engine_initializer.hpp"

namespace glow {

EngineInitializer::EngineInitializer(const GraphicsAPI api)
    : mSDL {api},
      mWindow {api}
{
  if (api == GraphicsAPI::OpenGL) {
    mGL.emplace(mWindow.get_handle());
  }

  mImGui.emplace(mWindow.get_handle(), api);
}

}  // namespace glow
