#pragma once

#include "graphics/graphics_api.hpp"

namespace gravel {

/// Simple RAII wrapper around an SDL instance.
class SDL final {
 public:
  explicit SDL(GraphicsApi api);

  ~SDL();

 private:
  GraphicsApi mAPI {};
};

}  // namespace gravel
