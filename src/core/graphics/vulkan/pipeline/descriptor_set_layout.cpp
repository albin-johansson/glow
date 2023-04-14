#include "descriptor_set_layout.hpp"

#include "graphics/vulkan/context.hpp"

namespace glow::vk {

void DescriptorSetLayoutDeleter::operator()(VkDescriptorSetLayout layout) noexcept
{
  vkDestroyDescriptorSetLayout(get_device(), layout, nullptr);
}

}  // namespace glow::vk