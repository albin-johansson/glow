#include "command_buffer.hpp"

#include "graphics/vulkan/cmd/command_pool.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/queue.hpp"
#include "graphics/vulkan/util/vk_call.hpp"

namespace glow::vk {
namespace {

/// Creates a temporary command buffer.
[[nodiscard]] auto _record_one_time_commands(VkCommandPool cmd_pool) -> VkCommandBuffer
{
  VkCommandBuffer cmd_buffer = allocate_command_buffer(cmd_pool);
  begin_command_buffer(cmd_buffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

  return cmd_buffer;
}

/// Executes the commands of a temporary command buffer, blocking until the work is done.
void _execute_one_time_commands(VkCommandPool cmd_pool, VkCommandBuffer cmd_buffer)
{
  end_command_buffer(cmd_buffer);

  VkQueue graphics_queue = get_graphics_queue();
  submit_to_queue(graphics_queue, cmd_buffer);
  wait_on_queue(graphics_queue);

  vkFreeCommandBuffers(get_device(), cmd_pool, 1, &cmd_buffer);
}

}  // namespace

void reset_command_buffer(VkCommandBuffer cmd_buffer)
{
  const VkCommandBufferResetFlags flags = 0;
  GLOW_VK_CALL(vkResetCommandBuffer(cmd_buffer, flags),
               "[VK] Could not reset command buffer");
}

void begin_command_buffer(VkCommandBuffer cmd_buffer,
                          const VkCommandBufferUsageFlags flags)
{
  const VkCommandBufferBeginInfo begin_info {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = nullptr,
      .flags = flags,
      .pInheritanceInfo = nullptr,
  };

  GLOW_VK_CALL(vkBeginCommandBuffer(cmd_buffer, &begin_info),
               "[VK] Could not begin command buffer");
}

void end_command_buffer(VkCommandBuffer cmd_buffer)
{
  GLOW_VK_CALL(vkEndCommandBuffer(cmd_buffer), "[VK] Could not end command buffer");
}

void execute_immediately(VkCommandPool cmd_pool, const UnaryCmdBufferFunc& func)
{
  VkCommandBuffer cmd_buffer = _record_one_time_commands(cmd_pool);
  func(cmd_buffer);
  _execute_one_time_commands(cmd_pool, cmd_buffer);
}

}  // namespace glow::vk
