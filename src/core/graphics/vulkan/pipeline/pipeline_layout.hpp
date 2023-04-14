#pragma once

#include <vulkan/vulkan.h>

#include "common/type/memory.hpp"

namespace glow::vk {

struct PipelineLayoutDeleter final {
  void operator()(VkPipelineLayout layout) noexcept;
};

using PipelineLayout = Unique<VkPipelineLayout_T, PipelineLayoutDeleter>;

}  // namespace glow::vk
