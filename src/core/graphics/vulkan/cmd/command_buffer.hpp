#pragma once

#include <vulkan/vulkan.h>

#include "common/primitives.hpp"
#include "common/type/vector.hpp"

namespace gravel::vlk {

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

}  // namespace gravel::vlk
