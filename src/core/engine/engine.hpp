#pragma once

#include <memory>  // unique_ptr

#include <SDL2/SDL.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "engine/engine_initializer.hpp"
#include "graphics/graphics_api.hpp"
#include "scene/scene.hpp"

namespace gravel {

GRAVEL_FORWARD_DECLARE_C(Backend);

class Engine final {
 public:
  GRAVEL_DELETE_COPY(Engine);
  GRAVEL_DELETE_MOVE(Engine);

  explicit Engine(GraphicsApi api);

  void start();

  void set_backend(std::unique_ptr<Backend> backend);

  [[nodiscard]] auto get_window() -> SDL_Window* { return mWindow; }

 private:
  EngineInitializer mInitializer;
  SDL_Window* mWindow {};
  float64 mCounterFreq {};
  float64 mFixedDelta {};
  int32 mMaxTicksPerFrame {5};
  std::unique_ptr<Backend> mBackend;
  Scene mScene;

  [[nodiscard]] auto query_counter() const -> float64;
};

}  // namespace gravel
