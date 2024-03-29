#include "context.hpp"

#include <fmt/format.h>
#include <glad/glad.h>

#include "common/debug/error.hpp"

namespace glow::gl {

void ContextDeleter::operator()(void* context) noexcept
{
  SDL_GL_DeleteContext(context);
}

Context::Context(SDL_Window* window)
    : mContext {SDL_GL_CreateContext(window)}
{
  if (!mContext) {
    throw Error {fmt::format("[GL] Failed to create context: {}", SDL_GetError())};
  }

  if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    throw Error {"[GL] Failed to initialize GLAD"};
  }
}

}  // namespace glow::gl
