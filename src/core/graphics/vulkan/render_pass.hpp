#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"

namespace gravel::vlk {

class RenderPass final {
 public:
  GRAVEL_DELETE_COPY(RenderPass);
  GRAVEL_DELETE_MOVE(RenderPass);

  RenderPass(VkDevice device, VkFormat swapchain_image_format);

  ~RenderPass();

  [[nodiscard]] auto get() -> VkRenderPass { return mRenderPass; }

 private:
  VkDevice mDevice {VK_NULL_HANDLE};
  VkRenderPass mRenderPass {VK_NULL_HANDLE};
};

}  // namespace gravel::vlk
