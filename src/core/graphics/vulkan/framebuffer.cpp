#include "framebuffer.hpp"

#include "graphics/vulkan/util.hpp"

namespace gravel::vlk {

auto create_framebuffer(VkDevice device,
                        VkRenderPass render_pass,
                        VkImageView image_view,
                        const VkExtent2D extent) -> VkFramebuffer
{
  const VkFramebufferCreateInfo create_info {
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,

      .pNext = nullptr,
      .flags = 0,

      .renderPass = render_pass,

      .attachmentCount = 1,
      .pAttachments = &image_view,

      .width = extent.width,
      .height = extent.height,

      .layers = 1,
  };

  VkFramebuffer framebuffer = VK_NULL_HANDLE;
  GRAVEL_VK_CALL(vkCreateFramebuffer(device, &create_info, nullptr, &framebuffer),
                 "[VK] Could not create framebuffer");

  return framebuffer;
}

}  // namespace gravel::vlk
