#include "graphics_api.hpp"

#include "common/debug/error.hpp"

namespace glow {

auto get_short_name(const GraphicsAPI api) -> StringView
{
  switch (api) {
    case GraphicsAPI::OpenGL:
      return "OpenGL";

    case GraphicsAPI::Vulkan:
      return "Vulkan";

    default:
      throw Error {"Unknown graphics API enumerator"};
  }
}

}  // namespace glow
