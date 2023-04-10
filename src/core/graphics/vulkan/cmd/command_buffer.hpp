#pragma once

#include <functional>  // function

#include <vulkan/vulkan.h>

#include "common/primitives.hpp"
#include "common/type/vector.hpp"

namespace gravel::vk {

[[nodiscard]] auto create_command_buffer(VkDevice device, VkCommandPool command_pool)
    -> VkCommandBuffer;

[[nodiscard]] auto create_command_buffers(VkDevice device,
                                          VkCommandPool command_pool,
                                          uint32 count) -> Vector<VkCommandBuffer>;

void reset_command_buffer(VkCommandBuffer command_buffer);

void begin_command_buffer(VkCommandBuffer command_buffer,
                          VkCommandBufferUsageFlags flags = 0);

void end_command_buffer(VkCommandBuffer command_buffer);

[[nodiscard]] auto record_one_time_commands() -> VkCommandBuffer;

void execute_one_time_commands(VkCommandBuffer cmd_buffer);

/// Records a one time command buffer, and executes it (and waits for it to complete).
void execute_immediately(const std::function<void(VkCommandBuffer)>& func);

}  // namespace gravel::vk
