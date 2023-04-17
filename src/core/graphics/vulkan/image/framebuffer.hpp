#pragma once

#include <vulkan/vulkan.h>

#include "common/type/memory.hpp"

namespace glow::vk {

struct FramebufferDeleter final {
  void operator()(VkFramebuffer framebuffer) noexcept;
};

using FramebufferPtr = Unique<VkFramebuffer_T, FramebufferDeleter>;

[[nodiscard]] auto create_framebuffer(VkRenderPass render_pass,
                                      VkImageView color_buffer,
                                      VkImageView depth_buffer,
                                      const VkExtent2D& extent) -> FramebufferPtr;

}  // namespace glow::vk
