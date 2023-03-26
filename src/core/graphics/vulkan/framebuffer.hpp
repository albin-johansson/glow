#pragma once

#include <vulkan/vulkan.h>

namespace gravel::vlk {

[[nodiscard]] auto create_framebuffer(VkDevice device,
                                      VkRenderPass render_pass,
                                      VkImageView image_view,
                                      VkExtent2D extent) -> VkFramebuffer;

}  // namespace gravel::vlk
