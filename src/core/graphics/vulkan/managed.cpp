#include "managed.hpp"

#include "graphics/vulkan/context.hpp"

namespace gravel::vlk {

void PipelineDeleter::operator()(VkPipeline pipeline) noexcept
{
  vkDestroyPipeline(get_device(), pipeline, nullptr);
}

void PipelineLayoutDeleter::operator()(VkPipelineLayout layout) noexcept
{
  vkDestroyPipelineLayout(get_device(), layout, nullptr);
}

void DescriptorSetLayoutDeleter::operator()(VkDescriptorSetLayout layout) noexcept
{
  vkDestroyDescriptorSetLayout(get_device(), layout, nullptr);
}

}  // namespace gravel::vlk