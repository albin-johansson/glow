#pragma once

#include <string>  // string

#include "glow/core/rhi/rhi.hpp"

namespace glow {

struct RenderDeviceInfo final {
  std::string api_name;
  std::string api_version;
  std::string renderer_name;
};

struct RenderDeviceFeatures final {
  bool sampler_anisotropy : 1 {};
};

/** Provides the top-level API for the render hardware interface (RHI). */
class IRenderDevice {
 public:
  virtual ~IRenderDevice() noexcept = default;

  [[nodiscard]] virtual auto begin_frame() -> bool = 0;

  virtual void end_frame() = 0;

  [[nodiscard]] virtual auto get_resource_manager() -> IResourceManager& = 0;
  [[nodiscard]] virtual auto get_resource_manager() const -> const IResourceManager& = 0;

  [[nodiscard]] virtual auto get_features() const -> RenderDeviceFeatures = 0;

  [[nodiscard]] virtual auto get_info() const -> RenderDeviceInfo = 0;
};

}  // namespace glow
