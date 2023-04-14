#include "descriptor.hpp"

namespace glow::vk {

auto create_descriptor_buffer_write(const uint32 binding,
                                    const VkDescriptorType type,
                                    const VkDescriptorBufferInfo* info,
                                    VkDescriptorSet dst) -> VkWriteDescriptorSet
{
  return VkWriteDescriptorSet {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = nullptr,
      .dstSet = dst,
      .dstBinding = binding,
      .dstArrayElement = 0,
      .descriptorCount = 1,
      .descriptorType = type,
      .pImageInfo = nullptr,
      .pBufferInfo = info,
      .pTexelBufferView = nullptr,
  };
}

auto create_descriptor_image_write(const uint32 binding,
                                   const VkDescriptorType type,
                                   const VkDescriptorImageInfo* info,
                                   VkDescriptorSet dst) -> VkWriteDescriptorSet
{
  return VkWriteDescriptorSet {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = nullptr,
      .dstSet = dst,
      .dstBinding = binding,
      .dstArrayElement = 0,
      .descriptorCount = 1,
      .descriptorType = type,
      .pImageInfo = info,
      .pBufferInfo = nullptr,
      .pTexelBufferView = nullptr,
  };
}

}  // namespace glow::vk
