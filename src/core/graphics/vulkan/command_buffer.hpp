#pragma once

#include <vulkan/vulkan.h>

namespace gravel::vlk {

[[nodiscard]] auto create_command_pool(VkDevice device,
                                       VkPhysicalDevice gpu,
                                       VkSurfaceKHR surface) -> VkCommandPool;

[[nodiscard]] auto create_command_buffer(VkDevice device, VkCommandPool command_pool)
    -> VkCommandBuffer;

void reset_command_buffer(VkCommandBuffer command_buffer);

void begin_command_buffer(VkCommandBuffer command_buffer);

}  // namespace gravel::vlk
