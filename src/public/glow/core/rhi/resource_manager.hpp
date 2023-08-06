#pragma once

#include "glow/core/prelude.hpp"
#include "glow/core/rhi/rhi.hpp"

namespace glow {

class GLOW_CORE_API IResourceManager {
 public:
  virtual ~IResourceManager() noexcept = default;

  /** Allocates, but does not initialize, a buffer resource. */
  [[nodiscard]] virtual auto allocate_buffer() -> RenderID = 0;

  /** Allocates, but does not initialize, a pipeline state object. */
  [[nodiscard]] virtual auto allocate_graphics_pipeline() -> RenderID = 0;

  [[nodiscard]] virtual auto get_buffer(RenderID id) -> IBuffer& = 0;
  [[nodiscard]] virtual auto get_buffer(RenderID id) const -> const IBuffer& = 0;

  [[nodiscard]] virtual auto get_graphics_pipeline(RenderID id) -> IGraphicsPipeline& = 0;
  [[nodiscard]] virtual auto get_graphics_pipeline(RenderID id) const
      -> const IGraphicsPipeline& = 0;
};

}  // namespace glow