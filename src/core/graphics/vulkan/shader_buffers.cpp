#include "shader_buffers.hpp"

namespace gravel::vk {

auto StaticMatrices::descriptor_buffer_info(VkBuffer buffer) -> VkDescriptorBufferInfo
{
  return VkDescriptorBufferInfo {
      .buffer = buffer,
      .offset = 0,
      .range = sizeof(StaticMatrices),
  };
}

auto StaticMatrices::descriptor_set_write(VkDescriptorSet set,
                                          const VkDescriptorBufferInfo* info)
    -> VkWriteDescriptorSet
{
  return VkWriteDescriptorSet {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = nullptr,

      .dstSet = set,

      .dstBinding = 0,
      .dstArrayElement = 0,

      .descriptorCount = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,

      .pImageInfo = nullptr,
      .pBufferInfo = info,
      .pTexelBufferView = nullptr,
  };
}

auto DynamicMatrices::push_constant_range(const uint32 offset,
                                          const VkShaderStageFlags stages)
    -> VkPushConstantRange
{
  return VkPushConstantRange {
      .stageFlags = stages,
      .offset = offset,
      .size = sizeof(DynamicMatrices),
  };
}

}  // namespace gravel::vk