#include "backend.hpp"

#include "common/debug/error.hpp"
#include "engine/backends/opengl_backend.hpp"
#include "engine/backends/vulkan_backend.hpp"

namespace glow {

auto create_backend(SDL_Window* window, const GraphicsAPI api) -> Unique<Backend>
{
  switch (api) {
    case GraphicsAPI::OpenGL:
      return std::make_unique<OpenGLBackend>(window);

    case GraphicsAPI::Vulkan:
      return std::make_unique<VulkanBackend>();

    default:
      throw Error {"Unsupported backend"};
  }
}

}  // namespace glow
