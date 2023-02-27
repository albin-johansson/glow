#pragma once

#include <SDL2/SDL.h>

namespace gravel::gl {

class OpenGLBackend final {
 public:
  explicit OpenGLBackend(SDL_Window* window);

  void update(float dt);

  void render();

  [[nodiscard]] auto should_quit() const -> bool { return mQuit; }

 private:
  SDL_Window* mWindow {};
  bool mQuit {false};
};

}  // namespace gravel::gl
