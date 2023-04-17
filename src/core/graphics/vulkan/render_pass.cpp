#include "render_pass.hpp"

#include "common/debug/assert.hpp"
#include "common/debug/error.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util/size.hpp"
#include "graphics/vulkan/util/vk_call.hpp"
#include "util/arrays.hpp"

namespace glow::vk {

void RenderPassDeleter::operator()(VkRenderPass render_pass) noexcept
{
  vkDestroyRenderPass(get_device(), render_pass, nullptr);
}

auto RenderPassBuilder::attachment(const VkFormat format,
                                   const VkImageLayout initial_layout,
                                   const VkImageLayout final_layout,
                                   const VkSampleCountFlagBits samples) -> Self&
{
  mAttachments.push_back(VkAttachmentDescription {
      .flags = 0,
      .format = format,
      .samples = samples,

      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,

      .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,

      .initialLayout = initial_layout,
      .finalLayout = final_layout,
  });

  return *this;
}

auto RenderPassBuilder::dependency(const uint32 src_subpass,
                                   const uint32 dst_subpass,
                                   const VkPipelineStageFlags src_stages,
                                   const VkPipelineStageFlags dst_stages,
                                   const VkAccessFlags src_access,
                                   const VkAccessFlags dst_access) -> Self&
{
  mDependencies.push_back(VkSubpassDependency {
      .srcSubpass = src_subpass,
      .dstSubpass = dst_subpass,

      .srcStageMask = src_stages,
      .dstStageMask = dst_stages,

      .srcAccessMask = src_access,
      .dstAccessMask = dst_access,

      .dependencyFlags = 0,
  });

  return *this;
}

auto RenderPassBuilder::dependency(const VkSubpassDependency& subpass_dependency) -> Self&
{
  mDependencies.push_back(subpass_dependency);
  return *this;
}

auto RenderPassBuilder::begin_subpass() -> Self&
{
  GRAVEL_ASSERT_MSG(!mCurrentSubpass.has_value(),
                    "Must call end_subpass after begin_subpass");

  mSubpasses.emplace_back();
  mCurrentSubpass = mSubpasses.size() - 1;

  return *this;
}

auto RenderPassBuilder::end_subpass() -> Self&
{
  GRAVEL_ASSERT_MSG(mCurrentSubpass.has_value(),
                    "Must call begin_subpass before end_subpass");

  auto& subpass = mSubpasses.at(mCurrentSubpass.value());
  subpass.description.colorAttachmentCount = u32_size(subpass.color_attachments);

  subpass.description = VkSubpassDescription {
      .flags = 0,
      .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,

      .inputAttachmentCount = u32_size(subpass.input_attachments),
      .pInputAttachments = !subpass.input_attachments.empty()  //
                               ? subpass.input_attachments.data()
                               : nullptr,

      .colorAttachmentCount = u32_size(subpass.color_attachments),
      .pColorAttachments = !subpass.color_attachments.empty()  //
                               ? subpass.color_attachments.data()
                               : nullptr,

      .pResolveAttachments = nullptr,
      .pDepthStencilAttachment = subpass.has_depth_attachment  //
                                     ? &subpass.depth_attachment
                                     : nullptr,

      .preserveAttachmentCount = 0,
      .pPreserveAttachments = nullptr,
  };

  mCurrentSubpass.reset();

  return *this;
}

auto RenderPassBuilder::color_attachment(const uint32 attachment) -> Self&
{
  auto& subpass = mSubpasses.at(mCurrentSubpass.value());

  subpass.color_attachments.push_back(VkAttachmentReference {
      .attachment = attachment,
      .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
  });

  return *this;
}

auto RenderPassBuilder::input_attachment(const uint32 attachment) -> Self&
{
  auto& subpass = mSubpasses.at(mCurrentSubpass.value());

  subpass.input_attachments.push_back(VkAttachmentReference {
      .attachment = attachment,
      .layout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
  });

  return *this;
}

auto RenderPassBuilder::depth_attachment(const uint32 attachment) -> Self&
{
  auto& subpass = mSubpasses.at(mCurrentSubpass.value());
  GRAVEL_ASSERT_MSG(!subpass.has_depth_attachment,
                    "Depth attachment can only be assigned once");

  subpass.has_depth_attachment = true;
  subpass.depth_attachment = VkAttachmentReference {
      .attachment = attachment,
      .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
  };

  mDepthAttachment = attachment;

  return *this;
}

auto RenderPassBuilder::build() const -> RenderPassInfo
{
  // The subpass descriptions have to be stored contiguously, so we use a temporary vector
  Vector<VkSubpassDescription> subpass_descriptions;
  subpass_descriptions.reserve(mSubpasses.size());

  for (const auto& subpass_info : mSubpasses) {
    subpass_descriptions.push_back(subpass_info.description);
  }

  const auto attachment_count = u32_size(mAttachments);
  const auto subpass_count = u32_size(mSubpasses);
  const auto dependency_count = u32_size(mDependencies);

  const VkRenderPassCreateInfo create_info {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,

      .attachmentCount = attachment_count,
      .pAttachments = (attachment_count > 0) ? mAttachments.data() : nullptr,

      .subpassCount = subpass_count,
      .pSubpasses = (subpass_count > 0) ? subpass_descriptions.data() : nullptr,

      .dependencyCount = dependency_count,
      .pDependencies = (dependency_count > 0) ? mDependencies.data() : nullptr,
  };

  VkRenderPass render_pass = VK_NULL_HANDLE;
  GRAVEL_VK_CALL(vkCreateRenderPass(get_device(), &create_info, nullptr, &render_pass),
                 "[VK] Could not create render pass");

  RenderPassInfo info;
  info.pass.reset(render_pass);
  info.clear_values.resize(attachment_count);
  info.depth_attachment = mDepthAttachment;

  for (auto& clear_value : info.clear_values) {
    clear_value.color = {.float32 = {0, 0, 0, 1}};
  }

  if (info.depth_attachment) {
    auto& clear_value = info.clear_values.at(*info.depth_attachment);
    clear_value.depthStencil = {.depth = 1, .stencil = 0};
  }

  return info;
}

void cmd_begin_render_pass(VkCommandBuffer cmd_buffer,
                           RenderPassInfo& render_pass_info,
                           VkFramebuffer framebuffer,
                           const VkExtent2D& framebuffer_extent)
{
  const VkRenderPassBeginInfo begin_info {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .pNext = nullptr,

      .renderPass = render_pass_info.pass.get(),
      .framebuffer = framebuffer,
      .renderArea = VkRect2D {.offset = {0, 0}, .extent = framebuffer_extent},

      .clearValueCount = u32_size(render_pass_info.clear_values),
      .pClearValues = render_pass_info.clear_values.data(),
  };

  vkCmdBeginRenderPass(cmd_buffer, &begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

}  // namespace glow::vk
