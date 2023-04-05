#include "command_buffer.hpp"

#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/physical_device.hpp"
#include "graphics/vulkan/util.hpp"

namespace gravel::vlk {

auto create_command_buffer(VkDevice device, VkCommandPool command_pool) -> VkCommandBuffer
{
  const VkCommandBufferAllocateInfo allocate_info {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = nullptr,
      .commandPool = command_pool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1,
  };

  VkCommandBuffer command_buffer {VK_NULL_HANDLE};
  GRAVEL_VK_CALL(vkAllocateCommandBuffers(device, &allocate_info, &command_buffer),
                 "[VK] Could not create command buffer");

  return command_buffer;
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

auto record_one_time_commands() -> VkCommandBuffer
{
  VkCommandBuffer cmd_buffer = create_command_buffer(get_device(), get_command_pool());
  begin_command_buffer(cmd_buffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

  return cmd_buffer;
}

void execute_one_time_commands(VkCommandBuffer cmd_buffer)
{
  end_command_buffer(cmd_buffer);

  const VkSubmitInfo submit_info {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = nullptr,

      .waitSemaphoreCount = 0,
      .pWaitSemaphores = nullptr,
      .pWaitDstStageMask = nullptr,

      .commandBufferCount = 1,
      .pCommandBuffers = &cmd_buffer,

      .signalSemaphoreCount = 0,
      .pSignalSemaphores = nullptr,
  };

  VkQueue graphics_queue = get_graphics_queue();
  GRAVEL_VK_CALL(vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE),
                 "[VK] Could not submit work to queue");
  GRAVEL_VK_CALL(vkQueueWaitIdle(graphics_queue),
                 "[VK] Could not wait for graphics queue");

  vkFreeCommandBuffers(get_device(), get_command_pool(), 1, &cmd_buffer);
}

void execute_immediately(const std::function<void(VkCommandBuffer)>& func)
{
  VkCommandBuffer cmd_buffer = record_one_time_commands();
  func(cmd_buffer);
  execute_one_time_commands(cmd_buffer);
}

}  // namespace gravel::vlk
