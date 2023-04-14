#pragma once

#include <vulkan/vulkan.h>

#include "common/type/memory.hpp"

namespace glow::vk {

struct DescriptorSetLayoutDeleter final {
  void operator()(VkDescriptorSetLayout layout) noexcept;
};

using DescriptorSetLayout = Unique<VkDescriptorSetLayout_T, DescriptorSetLayoutDeleter>;

}  // namespace glow::vk
