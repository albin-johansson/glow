#include "sdl.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <fmt/format.h>

#include "common/debug/error.hpp"
#include "common/predef.hpp"

namespace gravel {
namespace {

void set_hints_for_opengl()
{
  // Request OpenGL 4.1 core
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

  int context_flags = 0;
  if constexpr (kDebugBuild) {
    context_flags |= SDL_GL_CONTEXT_DEBUG_FLAG;
  }

  if constexpr (kIsMacOS) {
    context_flags |= SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, context_flags);

  // Enable double buffering
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  // Require hardware acceleration
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

  // Request sRGB capable framebuffers
  SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

  // Minimum amount of bits per pixel in the depth buffer
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  // Minimum amount of bits per pixel for each color buffer channel
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
}

}  // namespace

SDL::SDL(const GraphicsApi api)
    : mAPI {api}
{
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    throw Error {fmt::format("Could not initialize SDL: {}", SDL_GetError())};
  }

  SDL_SetHint(SDL_HINT_APP_NAME, "Gravel");

  if (mAPI == GraphicsApi::OpenGL) {
    set_hints_for_opengl();
  }
  else if (mAPI == GraphicsApi::Vulkan) {
    if (SDL_Vulkan_LoadLibrary(nullptr) == -1) {
      throw Error {fmt::format("Could not load Vulkan library: {}", SDL_GetError())};
    }
  }
}

SDL::~SDL()
{
  if (mAPI == GraphicsApi::Vulkan) {
    SDL_Vulkan_UnloadLibrary();
  }
  SDL_Quit();
}

}  // namespace gravel