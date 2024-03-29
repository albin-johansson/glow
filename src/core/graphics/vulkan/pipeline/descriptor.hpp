#pragma once

#include <vulkan/vulkan.h>

#include "common/primitives.hpp"

namespace glow::vk {

[[nodiscard]] auto create_descriptor_buffer_write(uint32 binding,
                                                  VkDescriptorType type,
                                                  const VkDescriptorBufferInfo* info,
                                                  VkDescriptorSet dst = VK_NULL_HANDLE)
    -> VkWriteDescriptorSet;

[[nodiscard]] auto create_descriptor_image_write(uint32 binding,
                                                 VkDescriptorType type,
                                                 const VkDescriptorImageInfo* info,
                                                 VkDescriptorSet dst = VK_NULL_HANDLE)
    -> VkWriteDescriptorSet;

}  // namespace glow::vk
