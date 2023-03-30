#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"

namespace gravel::vlk {

class PipelineCache final {
 public:
  GRAVEL_DELETE_COPY(PipelineCache);

  PipelineCache();

  ~PipelineCache() noexcept;

  PipelineCache(PipelineCache&& other) noexcept;

  auto operator=(PipelineCache&& other) noexcept -> PipelineCache&;

  [[nodiscard]] auto get() -> VkPipelineCache { return mCache; }

 private:
  VkPipelineCache mCache {VK_NULL_HANDLE};

  void dispose() noexcept;
};

}  // namespace gravel::vlk
