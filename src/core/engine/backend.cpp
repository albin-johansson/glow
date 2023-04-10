#include "backend.hpp"

#include "common/debug/error.hpp"
#include "graphics/opengl/opengl_backend.hpp"
#include "graphics/vulkan/vulkan_backend.hpp"

namespace gravel {

auto create_backend(SDL_Window* window, const GraphicsApi api) -> Unique<Backend>
{
  switch (api) {
    case GraphicsApi::OpenGL:
      return std::make_unique<gl::OpenGLBackend>(window);

    case GraphicsApi::Vulkan:
      return std::make_unique<vk::VulkanBackend>();

    default:
      throw Error {"Unsupported backend"};
  }
}

}  // namespace gravel
