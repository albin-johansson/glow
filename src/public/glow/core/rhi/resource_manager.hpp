#pragma once

#include "glow/core/prelude.hpp"
#include "glow/core/rhi/rhi.hpp"

namespace glow {

class IResourceManager {
 public:
  virtual ~IResourceManager() noexcept = default;

  /** Allocates, but does not initialize, a buffer resource. */
  [[nodiscard]] virtual auto allocate_buffer() -> RenderID = 0;
};

}  // namespace glow