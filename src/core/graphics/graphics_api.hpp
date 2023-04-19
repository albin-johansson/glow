#pragma once

#include "common/type/string.hpp"

namespace glow {

enum class GraphicsAPI {
  OpenGL,
  Vulkan
};

[[nodiscard]] auto get_short_name(GraphicsAPI api) -> StringView;

}  // namespace glow