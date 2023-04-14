#pragma once

#include <vulkan/vulkan.h>

#include "common/type/memory.hpp"

namespace glow::vk {

struct PipelineCacheDeleter final {
  void operator()(VkPipelineCache cache) noexcept;
};

using PipelineCache = Unique<VkPipelineCache_T, PipelineCacheDeleter>;

[[nodiscard]] auto create_pipeline_cache() -> PipelineCache;

}  // namespace glow::vk
