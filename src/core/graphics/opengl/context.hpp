#pragma once

#include <SDL2/SDL.h>

#include "common/type/memory.hpp"

namespace gravel::gl {

struct ContextDeleter final {
  void operator()(void* context) noexcept;
};

class Context final {
 public:
  explicit Context(SDL_Window* window);

  [[nodiscard]] auto get_handle() -> SDL_GLContext { return mContext.get(); }

 private:
  Unique<void, ContextDeleter> mContext;
};

}  // namespace gravel::gl
