#pragma once

#include <memory>  // unique_ptr

#include "glow/core/prelude.hpp"
#include "glow/core/rhi/rhi.hpp"

namespace glow {

class GLOW_CORE_API IRenderContext {
 public:
  using UniqueRenderDevice = std::unique_ptr<IRenderDevice>;

  virtual ~IRenderContext() noexcept = default;

  virtual void add_render_device(UniqueRenderDevice device) = 0;
};

}  // namespace glow
