#pragma once

#include "graphics/graphics_api.hpp"

namespace glow {

/// Simple RAII wrapper around an SDL instance.
class SDL final {
 public:
  explicit SDL(GraphicsAPI api);

  ~SDL();

 private:
  GraphicsAPI mAPI {};
};

}  // namespace glow
