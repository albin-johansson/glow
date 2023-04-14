#include "command_buffer.hpp"

#include "graphics/vulkan/cmd/command_pool.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/queue.hpp"
#include "graphics/vulkan/util/vk_call.hpp"

namespace glow::vk {
namespace {

[[nodiscard]] auto record_one_time_commands() -> VkCommandBuffer
{
  VkCommandBuffer cmd_buffer = allocate_command_buffer(get_command_pool());
  begin_command_buffer(cmd_buffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

  return cmd_buffer;
}

void execute_one_time_commands(VkCommandBuffer cmd_buffer)
{
  end_command_buffer(cmd_buffer);

  VkQueue graphics_queue = get_graphics_queue();
  submit_to_queue(graphics_queue, cmd_buffer);
  wait_on_queue(graphics_queue);

  vkFreeCommandBuffers(get_device(), get_command_pool(), 1, &cmd_buffer);
}

}  // namespace

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

void execute_immediately(const UnaryCmdBufferFunc& func)
{
  VkCommandBuffer cmd_buffer = record_one_time_commands();
  func(cmd_buffer);
  execute_one_time_commands(cmd_buffer);
}

}  // namespace glow::vk
