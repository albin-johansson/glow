#include "command_pool.hpp"

#include "common/debug/assert.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/physical_device.hpp"
#include "graphics/vulkan/util/vk_call.hpp"

namespace gravel::vk {

void CommandPoolDeleter::operator()(VkCommandPool pool) noexcept
{
  vkDestroyCommandPool(get_device(), pool, nullptr);
}

CommandPool::CommandPool()
{
  GRAVEL_ASSERT(get_surface() != VK_NULL_HANDLE);
  GRAVEL_ASSERT(get_device() != VK_NULL_HANDLE);
  GRAVEL_ASSERT(get_gpu() != VK_NULL_HANDLE);

  const auto queue_family_indices = get_queue_family_indices(get_gpu(), get_surface());

  const VkCommandPoolCreateInfo create_info {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = queue_family_indices.graphics_family.value(),
  };

  VkCommandPool pool = VK_NULL_HANDLE;
  GRAVEL_VK_CALL(vkCreateCommandPool(get_device(), &create_info, nullptr, &pool),
                 "[VK] Could not create command pool");

  mCommandPool.reset(pool);
  set_command_pool(pool);
}

auto CommandPool::allocate_command_buffer() -> VkCommandBuffer
{
  const VkCommandBufferAllocateInfo allocate_info {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = nullptr,
      .commandPool = mCommandPool.get(),
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1,
  };

  VkCommandBuffer buffer {VK_NULL_HANDLE};
  GRAVEL_VK_CALL(vkAllocateCommandBuffers(get_device(), &allocate_info, &buffer),
                 "[VK] Could not create command buffer");

  return buffer;
}

auto CommandPool::allocate_command_buffers(uint32 count) -> Vector<VkCommandBuffer>
{
  const VkCommandBufferAllocateInfo allocate_info {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = nullptr,
      .commandPool = mCommandPool.get(),
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = count,
  };

  Vector<VkCommandBuffer> buffers;
  buffers.resize(count);

  GRAVEL_VK_CALL(vkAllocateCommandBuffers(get_device(), &allocate_info, buffers.data()),
                 "[VK] Could not create command buffers");

  return buffers;
}

}  // namespace gravel::vk
