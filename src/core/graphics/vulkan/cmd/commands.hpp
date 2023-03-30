#pragma once

#include <vulkan/vulkan.h>

#include "common/primitives.hpp"

namespace gravel::vlk::cmd {

void set_viewport(VkCommandBuffer command_buffer, VkExtent2D extent);

void set_scissor(VkCommandBuffer command_buffer, VkOffset2D offset, VkExtent2D extent);

void copy_buffer(VkCommandBuffer command_buffer,
                 VkBuffer src,
                 VkBuffer dst,
                 usize data_size);

}  // namespace gravel::vlk::cmd
