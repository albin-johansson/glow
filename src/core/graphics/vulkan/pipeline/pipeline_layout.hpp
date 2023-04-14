#pragma once

#include <vulkan/vulkan.h>

#include "common/type/memory.hpp"

namespace gravel::vk {

struct PipelineLayoutDeleter final {
  void operator()(VkPipelineLayout layout) noexcept;
};

using PipelineLayout = Unique<VkPipelineLayout_T, PipelineLayoutDeleter>;

}  // namespace gravel::vk
