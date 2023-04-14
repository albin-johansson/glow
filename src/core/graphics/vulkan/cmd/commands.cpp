#include "commands.hpp"

namespace glow::vk::cmd {

void set_viewport(VkCommandBuffer command_buffer, VkExtent2D extent)
{
  const VkViewport viewport {
      .x = 0.0f,
      .y = 0.0f,

      .width = static_cast<float>(extent.width),
      .height = static_cast<float>(extent.height),

      .minDepth = 0.0f,
      .maxDepth = 1.0f,
  };

  vkCmdSetViewport(command_buffer, 0, 1, &viewport);
}

void set_scissor(VkCommandBuffer command_buffer, VkOffset2D offset, VkExtent2D extent)
{
  const VkRect2D scissor {
      .offset = offset,
      .extent = extent,
  };

  vkCmdSetScissor(command_buffer, 0, 1, &scissor);
}

void copy_buffer(VkCommandBuffer command_buffer,
                 VkBuffer src,
                 VkBuffer dst,
                 const usize data_size)
{
  const VkBufferCopy region {
      .srcOffset = 0,
      .dstOffset = 0,
      .size = data_size,
  };

  vkCmdCopyBuffer(command_buffer, src, dst, 1, &region);
}

}  // namespace glow::vk::cmd
