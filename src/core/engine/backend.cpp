#include "backend.hpp"

#include "common/debug/error.hpp"
#include "engine/backends/opengl_backend.hpp"
#include "engine/backends/vulkan_backend.hpp"

namespace glow {

auto create_backend(SDL_Window* window, const GraphicsApi api) -> Unique<Backend>
{
  switch (api) {
    case GraphicsApi::OpenGL:
      return std::make_unique<OpenGLBackend>(window);

    case GraphicsApi::Vulkan:
      return std::make_unique<VulkanBackend>();

    default:
      throw Error {"Unsupported backend"};
  }
}

}  // namespace glow
