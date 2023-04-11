#pragma once

#include <functional>  // function

#include <vulkan/vulkan.h>

#include "common/primitives.hpp"

namespace gravel::vk {

using UnaryCmdBufferFunc = std::function<void(VkCommandBuffer)>;

void reset_command_buffer(VkCommandBuffer command_buffer);

void begin_command_buffer(VkCommandBuffer command_buffer,
                          VkCommandBufferUsageFlags flags = 0);

void end_command_buffer(VkCommandBuffer command_buffer);

/// Records a one time command buffer, and executes it (and waits for it to complete).
void execute_immediately(const UnaryCmdBufferFunc& func);

}  // namespace gravel::vk
