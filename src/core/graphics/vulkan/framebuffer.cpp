#include "framebuffer.hpp"

#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util.hpp"
#include "util/arrays.hpp"

namespace gravel::vk {

Framebuffer::Framebuffer(VkRenderPass render_pass,
                         VkImageView image_view,
                         VkImageView depth_view,
                         const VkExtent2D extent)
    : mExtent {extent}
{
  const VkImageView attachments[] {
      image_view,
      depth_view,
  };

  const VkFramebufferCreateInfo create_info {
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,

      .pNext = nullptr,
      .flags = 0,

      .renderPass = render_pass,

      .attachmentCount = array_length(attachments),
      .pAttachments = attachments,

      .width = mExtent.width,
      .height = mExtent.height,

      .layers = 1,
  };

  GRAVEL_VK_CALL(vkCreateFramebuffer(get_device(), &create_info, nullptr, &mFramebuffer),
                 "[VK] Could not create framebuffer");
}

Framebuffer::~Framebuffer() noexcept
{
  dispose();
}

void Framebuffer::dispose() noexcept
{
  if (mFramebuffer != VK_NULL_HANDLE) {
    vkDestroyFramebuffer(get_device(), mFramebuffer, nullptr);
  }
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept
    : mFramebuffer {other.mFramebuffer},
      mExtent {other.mExtent}
{
  other.mFramebuffer = VK_NULL_HANDLE;
}

auto Framebuffer::operator=(Framebuffer&& other) noexcept -> Framebuffer&
{
  if (this != &other) {
    dispose();

    mFramebuffer = other.mFramebuffer;
    mExtent = other.mExtent;

    other.mFramebuffer = VK_NULL_HANDLE;
  }

  return *this;
}

}  // namespace gravel::vk
