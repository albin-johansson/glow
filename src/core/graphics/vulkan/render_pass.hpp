#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/maybe.hpp"
#include "common/type/memory.hpp"
#include "common/type/vector.hpp"

namespace glow::vk {

GRAVEL_FORWARD_DECLARE_C(Framebuffer);

struct RenderPassDeleter final {
  void operator()(VkRenderPass render_pass) noexcept;
};

using RenderPass = Unique<VkRenderPass_T, RenderPassDeleter>;

struct RenderPassInfo final {
  RenderPass pass;
  Vector<VkClearValue> clear_values;  ///< Buffer of clear values for each attachment.
  Maybe<uint32> depth_attachment;     ///< Index of depth attachment.
};

/// Used to aid the construction of render passes.
class RenderPassBuilder final {
  struct SubpassInfo final {
    Vector<VkAttachmentReference> color_attachments;
    Vector<VkAttachmentReference> input_attachments;
    VkAttachmentReference depth_attachment {};
    VkSubpassDescription description {};
    bool has_depth_attachment : 1 {false};
  };

 public:
  using Self = RenderPassBuilder;

  /// Adds an attachment to the render pass (independent of the current subpass).
  auto attachment(VkFormat format,
                  VkImageLayout initial_layout,
                  VkImageLayout final_layout,
                  VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT) -> Self&;

  /// Registers a subpass dependency.
  auto dependency(uint32 src_subpass,
                  uint32 dst_subpass,
                  VkPipelineStageFlags src_stages,
                  VkPipelineStageFlags dst_stages,
                  VkAccessFlags src_access = 0,
                  VkAccessFlags dst_access = 0) -> Self&;

  /// Marks the start of a new subpass, call this every time a new subpass is described.
  auto begin_subpass() -> Self&;

  /// Marks the end of the current subpass.
  auto end_subpass() -> Self&;

  /// Marks the attachment at the specified index as a color attachment in the subpass.
  auto color_attachment(uint32 attachment) -> Self&;

  /// Marks the attachment at the specified index as an input attachment in the subpass.
  auto input_attachment(uint32 attachment) -> Self&;

  /// Marks the attachment at the specified index as a depth attachment in the subpass.
  auto depth_attachment(uint32 attachment) -> Self&;

  /// Attempts to construct the described render pass.
  auto build() const -> RenderPassInfo;

 private:
  Vector<VkAttachmentDescription> mAttachments;
  Vector<VkSubpassDependency> mDependencies;
  Vector<SubpassInfo> mSubpasses;
  Maybe<usize> mCurrentSubpass;
  Maybe<uint32> mDepthAttachment;
};

void cmd_begin_render_pass(VkCommandBuffer cmd_buffer,
                           RenderPassInfo& render_pass_info,
                           VkFramebuffer framebuffer,
                           const VkExtent2D& framebuffer_extent);

}  // namespace glow::vk
