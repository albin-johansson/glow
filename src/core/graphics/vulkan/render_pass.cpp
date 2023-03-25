#include "render_pass.hpp"

#include "common/debug/error.hpp"
#include "util/arrays.hpp"

namespace gravel::vlk {

RenderPass::RenderPass(VkDevice device, const VkFormat swapchain_image_format)
    : mDevice {device}
{
  const VkAttachmentDescription attachment_descriptions[] {
      VkAttachmentDescription {
          .flags = 0,
          .format = swapchain_image_format,
          .samples = VK_SAMPLE_COUNT_1_BIT,

          .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
          .storeOp = VK_ATTACHMENT_STORE_OP_STORE,

          .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
          .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,

          .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
          .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
      },
  };

  const VkAttachmentReference color_attachments[] {
      VkAttachmentReference {
          .attachment = 0,
          .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      },
  };

  const VkSubpassDescription subpasses[] {
      VkSubpassDescription {
          .flags = 0,
          .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,

          .inputAttachmentCount = 0,
          .pInputAttachments = nullptr,

          .colorAttachmentCount = array_length(color_attachments),
          .pColorAttachments = color_attachments,

          .pResolveAttachments = nullptr,
          .pDepthStencilAttachment = nullptr,

          .preserveAttachmentCount = 0,
          .pPreserveAttachments = nullptr,
      },
  };

  const VkSubpassDependency subpass_dependencies[] {
      VkSubpassDependency {
          .srcSubpass = VK_SUBPASS_EXTERNAL,
          .dstSubpass = 0,

          .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
          .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,

          .srcAccessMask = 0,
          .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,

          .dependencyFlags = 0,
      },
  };

  const VkRenderPassCreateInfo render_pass_create_info {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,

      .attachmentCount = array_length(attachment_descriptions),
      .pAttachments = attachment_descriptions,

      .subpassCount = array_length(subpasses),
      .pSubpasses = subpasses,

      .dependencyCount = array_length(subpass_dependencies),
      .pDependencies = subpass_dependencies,
  };

  if (vkCreateRenderPass(mDevice, &render_pass_create_info, nullptr, &mRenderPass) !=
      VK_SUCCESS) {
    throw Error {"[VK] Could not create render pass"};
  }
}

RenderPass::~RenderPass()
{
  vkDestroyRenderPass(mDevice, mRenderPass, nullptr);
}

}  // namespace gravel::vlk
