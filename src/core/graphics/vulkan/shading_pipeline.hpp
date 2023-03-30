#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/type/vector.hpp"
#include "graphics/vulkan/buffer.hpp"
#include "graphics/vulkan/pipeline/descriptor_pool.hpp"

namespace gravel::vlk {

class ShadingPipeline final {
 public:
  GRAVEL_DELETE_COPY(ShadingPipeline);
  GRAVEL_DELETE_MOVE(ShadingPipeline);

  ShadingPipeline(VkRenderPass render_pass, VkExtent2D swapchain_extent);

  ~ShadingPipeline();

  void bind(VkCommandBuffer command_buffer, usize frame_index);

  void update_descriptor_set(usize frame_index);

  [[nodiscard]] auto get_matrix_ubo(usize frame_index) -> Buffer&;

  [[nodiscard]] auto get_material_ubo(usize frame_index) -> Buffer&;

  [[nodiscard]] auto get() -> VkPipeline { return mPipeline; }

 private:
  DescriptorPool mDescriptorPool;
  VkDescriptorSetLayout mDescriptorSetLayout {VK_NULL_HANDLE};
  Vector<VkDescriptorSet> mDescriptorSets;
  Vector<Buffer> mMatrixUBOs;
  Vector<Buffer> mMaterialUBOs;
  VkPipelineLayout mPipelineLayout {VK_NULL_HANDLE};
  VkPipeline mPipeline {VK_NULL_HANDLE};

  void create_descriptor_set_layout();
  void create_descriptor_sets();
  void create_pipeline_layout();
  void create_pipeline(VkRenderPass render_pass, VkExtent2D swapchain_extent);
};

}  // namespace gravel::vlk
