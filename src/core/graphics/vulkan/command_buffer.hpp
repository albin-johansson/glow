#pragma once

#include <vulkan/vulkan.h>

namespace gravel::vlk {

void reset_command_buffer(VkCommandBuffer command_buffer);

void begin_command_buffer(VkCommandBuffer command_buffer);

}  // namespace gravel::vlk
