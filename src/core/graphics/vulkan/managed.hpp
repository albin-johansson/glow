#pragma once

#include <vulkan/vulkan.h>

#include "common/type/memory.hpp"

namespace gravel::vlk {

struct PipelineDeleter final {
  void operator()(VkPipeline pipeline) noexcept;
};

struct PipelineLayoutDeleter final {
  void operator()(VkPipelineLayout layout) noexcept;
};

struct DescriptorSetLayoutDeleter final {
  void operator()(VkDescriptorSetLayout layout) noexcept;
};

using Pipeline = Unique<VkPipeline_T, PipelineDeleter>;
using PipelineLayout = Unique<VkPipelineLayout_T, PipelineLayoutDeleter>;
using DescriptorSetLayout = Unique<VkDescriptorSetLayout_T, DescriptorSetLayoutDeleter>;

}  // namespace gravel::vlk
