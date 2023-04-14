#include "pipeline_layout.hpp"

#include "graphics/vulkan/context.hpp"

namespace gravel::vk {

void PipelineLayoutDeleter::operator()(VkPipelineLayout layout) noexcept
{
  vkDestroyPipelineLayout(get_device(), layout, nullptr);
}

}  // namespace gravel::vk
