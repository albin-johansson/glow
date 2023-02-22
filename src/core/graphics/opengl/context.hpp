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

  [[nodiscard]] auto get_handle() -> SDL_GLContext { return mContext.get(); }

 private:
  std::unique_ptr<void, ContextDeleter> mContext;
};

}  // namespace gravel::gl
