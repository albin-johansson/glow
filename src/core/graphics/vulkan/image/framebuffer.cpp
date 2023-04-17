#include "framebuffer.hpp"

#include <array>  // size

#include "common/debug/assert.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util/vk_call.hpp"

namespace glow::vk {

void FramebufferDeleter::operator()(VkFramebuffer framebuffer) noexcept
{
  vkDestroyFramebuffer(get_device(), framebuffer, nullptr);
}

auto create_framebuffer(VkRenderPass render_pass,
                        VkImageView color_buffer,
                        VkImageView depth_buffer,
                        const VkExtent2D& extent) -> FramebufferPtr
{
  GRAVEL_ASSERT(render_pass != VK_NULL_HANDLE);
  GRAVEL_ASSERT(color_buffer != VK_NULL_HANDLE);
  GRAVEL_ASSERT(depth_buffer != VK_NULL_HANDLE);

  const VkImageView attachments[] = {
      color_buffer,
      depth_buffer,
  };

  const VkFramebufferCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .renderPass = render_pass,
      .attachmentCount = std::size(attachments),
      .pAttachments = attachments,
      .width = extent.width,
      .height = extent.height,
      .layers = 1,
  };

  VkFramebuffer framebuffer = VK_NULL_HANDLE;
  GRAVEL_VK_CALL(vkCreateFramebuffer(get_device(), &create_info, nullptr, &framebuffer),
                 "[VK] Could not create framebuffer");

  return FramebufferPtr {framebuffer};
}

}  // namespace glow::vk
