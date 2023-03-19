#pragma once

#include "common/type/maybe.hpp"
#include "common/type/string.hpp"

namespace gravel {

/// Context component with information about a renderer backend.
struct RendererInfo final {
  String api;       ///< API name.
  String renderer;  ///< Name of the renderer, e.g. the graphics card.
  String vendor;    ///< Name of the platform vendor.
  String version;   ///< Version or revision number.
};

}  // namespace gravel
