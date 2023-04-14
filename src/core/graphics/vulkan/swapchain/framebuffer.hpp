#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"

namespace glow::vk {

class Framebuffer final {
 public:
  GRAVEL_DELETE_COPY(Framebuffer);

  Framebuffer(VkRenderPass render_pass,
              VkImageView image_view,
              VkImageView depth_view,
              VkExtent2D extent);

  ~Framebuffer() noexcept;

  Framebuffer(Framebuffer&& other) noexcept;

  auto operator=(Framebuffer&& other) noexcept -> Framebuffer&;

  [[nodiscard]] auto get() -> VkFramebuffer { return mFramebuffer; }

 private:
  VkFramebuffer mFramebuffer {VK_NULL_HANDLE};
  VkExtent2D mExtent {};

  void dispose() noexcept;
};

}  // namespace glow::vk
