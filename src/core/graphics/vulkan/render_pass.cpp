#include "render_pass.hpp"

#include "common/debug/error.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/framebuffer.hpp"
#include "graphics/vulkan/util.hpp"
#include "util/arrays.hpp"

namespace gravel::vlk {

RenderPass::RenderPass(const VkFormat swapchain_image_format)
{
  const VkAttachmentDescription attachments[] {
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
      // Depth attachment
      VkAttachmentDescription {
          .flags = 0,
          .format = VK_FORMAT_D32_SFLOAT_S8_UINT,
          .samples = VK_SAMPLE_COUNT_1_BIT,

          .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
          .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,

          .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
          .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,

          .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
          .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
      }};

  const VkAttachmentReference color_attachments[] {
      VkAttachmentReference {
          .attachment = 0,
          .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      },
  };

  const VkAttachmentReference depth_attachment {
      .attachment = 1,
      .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
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
          .pDepthStencilAttachment = &depth_attachment,

          .preserveAttachmentCount = 0,
          .pPreserveAttachments = nullptr,
      },
  };

  const VkSubpassDependency subpass_dependencies[] {
      VkSubpassDependency {
          .srcSubpass = VK_SUBPASS_EXTERNAL,
          .dstSubpass = 0,

          .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                          VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
          .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                          VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,

          .srcAccessMask = 0,
          .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                           VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,

          .dependencyFlags = 0,
      },
  };

  const VkRenderPassCreateInfo render_pass_create_info {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,

      .attachmentCount = array_length(attachments),
      .pAttachments = attachments,

      .subpassCount = array_length(subpasses),
      .pSubpasses = subpasses,

      .dependencyCount = array_length(subpass_dependencies),
      .pDependencies = subpass_dependencies,
  };

  GRAVEL_VK_CALL(vkCreateRenderPass(get_device(),  //
                                    &render_pass_create_info,
                                    nullptr,
                                    &mRenderPass),
                 "[VK] Could not create render pass");
}

RenderPass::~RenderPass()
{
  vkDestroyRenderPass(get_device(), mRenderPass, nullptr);
}

void RenderPass::begin(VkCommandBuffer command_buffer,
                       VkFramebuffer framebuffer,
                       const VkExtent2D framebuffer_extent)
{
  VkClearValue clear_values[2] {};
  clear_values[0].color = {.float32 = {0.0f, 0.0f, 0.0f, 1.0f}};
  clear_values[1].depthStencil = {.depth = 1.0f, .stencil = 0};

  const VkRect2D render_area {
      .offset = VkOffset2D {0, 0},
      .extent = framebuffer_extent,
  };

  const VkRenderPassBeginInfo render_pass_begin_info {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .pNext = nullptr,

      .renderPass = mRenderPass,
      .framebuffer = framebuffer,
      .renderArea = render_area,

      .clearValueCount = array_length(clear_values),
      .pClearValues = clear_values,
  };

  vkCmdBeginRenderPass(command_buffer,
                       &render_pass_begin_info,
                       VK_SUBPASS_CONTENTS_INLINE);
}

}  // namespace gravel::vlk
