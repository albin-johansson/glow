#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"

namespace gravel::vk {

GRAVEL_FORWARD_DECLARE_C(Framebuffer);

class RenderPass final {
 public:
  GRAVEL_DELETE_COPY(RenderPass);
  GRAVEL_DELETE_MOVE(RenderPass);

  explicit RenderPass(VkFormat swapchain_image_format);

  ~RenderPass();

  void begin(VkCommandBuffer command_buffer,
             VkFramebuffer framebuffer,
             VkExtent2D framebuffer_extent);

  [[nodiscard]] auto get() -> VkRenderPass { return mRenderPass; }

 private:
  VkRenderPass mRenderPass {VK_NULL_HANDLE};
};

}  // namespace gravel::vk
