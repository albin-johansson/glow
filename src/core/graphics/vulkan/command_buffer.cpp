#include "command_buffer.hpp"

#include "graphics/vulkan/physical_device.hpp"
#include "graphics/vulkan/util.hpp"

namespace gravel::vlk {

auto create_command_pool(VkDevice device, VkPhysicalDevice gpu, VkSurfaceKHR surface)
    -> VkCommandPool
{
  const auto queue_family_indices = get_queue_family_indices(gpu, surface);

  const VkCommandPoolCreateInfo create_info {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = queue_family_indices.graphics_family.value(),
  };

  VkCommandPool command_pool = VK_NULL_HANDLE;
  GRAVEL_VK_CALL(vkCreateCommandPool(device, &create_info, nullptr, &command_pool),
                 "[VK] Could not create command pool");

  return command_pool;
}

auto create_command_buffers(VkDevice device,
                            VkCommandPool command_pool,
                            const uint32 count) -> Vector<VkCommandBuffer>
{
  const VkCommandBufferAllocateInfo allocate_info {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = nullptr,
      .commandPool = command_pool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = count,
  };

  Vector<VkCommandBuffer> command_buffers;
  command_buffers.resize(count);

  GRAVEL_VK_CALL(vkAllocateCommandBuffers(device, &allocate_info, command_buffers.data()),
                 "[VK] Could not create command buffers");

  return command_buffers;
}

void reset_command_buffer(VkCommandBuffer command_buffer)
{
  const VkCommandBufferResetFlags command_buffer_reset_flags = 0;
  GRAVEL_VK_CALL(vkResetCommandBuffer(command_buffer, command_buffer_reset_flags),
                 "[VK] Could not reset command buffer");
}

void begin_command_buffer(VkCommandBuffer command_buffer,
                          const VkCommandBufferUsageFlags flags)
{
  const VkCommandBufferBeginInfo command_buffer_begin_info {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = nullptr,
      .flags = flags,
      .pInheritanceInfo = nullptr,
  };

  GRAVEL_VK_CALL(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info),
                 "[VK] Could not begin command buffer");
}

void end_command_buffer(VkCommandBuffer command_buffer)
{
  GRAVEL_VK_CALL(vkEndCommandBuffer(command_buffer), "[VK] Could not end command buffer");
}

}  // namespace gravel::vlk
