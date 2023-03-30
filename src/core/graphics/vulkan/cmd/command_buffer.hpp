#pragma once

#include <vulkan/vulkan.h>

#include "common/primitives.hpp"
#include "common/type/vector.hpp"

namespace gravel::vlk {

[[nodiscard]] auto create_command_buffers(VkDevice device,
                                          VkCommandPool command_pool,
                                          uint32 count) -> Vector<VkCommandBuffer>;

void reset_command_buffer(VkCommandBuffer command_buffer);

void begin_command_buffer(VkCommandBuffer command_buffer,
                          VkCommandBufferUsageFlags flags = 0);

void end_command_buffer(VkCommandBuffer command_buffer);

}  // namespace gravel::vlk
