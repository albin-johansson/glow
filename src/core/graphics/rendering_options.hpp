#pragma once

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/map.hpp"

namespace glow {

enum class RenderingOption {
  VSync,
  DepthTest,
  FaceCulling,
  Wireframe,
  Blending
};

/// Context component for various rendering options.
struct RenderingOptions final {
  GLOW_MOVE_ONLY_COMPONENT(RenderingOptions);
  HashMap<RenderingOption, bool> options;

  [[nodiscard]] auto test(const RenderingOption option) const -> bool
  {
    return options.at(option);
  }
};

}  // namespace glow
