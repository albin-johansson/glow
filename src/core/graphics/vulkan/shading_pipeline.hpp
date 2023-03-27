#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"

namespace gravel::vlk {

class ShadingPipeline final {
 public:
  GRAVEL_DELETE_COPY(ShadingPipeline);
  GRAVEL_DELETE_MOVE(ShadingPipeline);

  explicit ShadingPipeline(VkDevice device,
                           VkRenderPass render_pass,
                           VkExtent2D swapchain_extent);

  ~ShadingPipeline();

  [[nodiscard]] auto get() -> VkPipeline { return mPipeline; }

 private:
  VkDevice mDevice {VK_NULL_HANDLE};
  VkDescriptorSetLayout mDescriptorSetLayout {VK_NULL_HANDLE};
  VkPipelineLayout mPipelineLayout {VK_NULL_HANDLE};
  VkPipeline mPipeline {VK_NULL_HANDLE};

  void create_descriptor_set_layout();
  void create_pipeline_layout();
  void create_pipeline(VkRenderPass render_pass, VkExtent2D swapchain_extent);
};

}  // namespace gravel::vlk
