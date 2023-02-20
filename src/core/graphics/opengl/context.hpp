#pragma once

#include <memory>  // unique_ptr

#include <SDL2/SDL.h>

namespace gravel::gl {

struct ContextDeleter final {
  void operator()(void* context) noexcept;
};

class Context final {
 public:
  explicit Context(SDL_Window* window);

 private:
  std::unique_ptr<void, ContextDeleter> mContext;
};

}  // namespace gravel::gl
