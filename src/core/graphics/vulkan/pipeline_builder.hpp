#pragma once

#include <vulkan/vulkan.h>

#include "common/primitives.hpp"
#include "common/type/array.hpp"
#include "common/type/maybe.hpp"
#include "common/type/vector.hpp"
#include "graphics/vulkan/shader_module.hpp"

namespace gravel::vk {

class DescriptorSetLayoutBuilder final {
 public:
  using Self = DescriptorSetLayoutBuilder;

  auto reset() -> Self&;

  auto descriptor(uint32 binding,
                  VkDescriptorType type,
                  VkShaderStageFlags stages,
                  uint32 count = 1) -> Self&;

  auto use_push_descriptors() -> Self&;

  [[nodiscard]] auto build() const -> VkDescriptorSetLayout;

 private:
  Vector<VkDescriptorSetLayoutBinding> mBindings;
  bool mUsePushDescriptors : 1 {false};
};

class PipelineLayoutBuilder final {
 public:
  using Self = PipelineLayoutBuilder;

  auto reset() -> Self&;

  auto descriptor_layout(VkDescriptorSetLayout set_layout) -> Self&;

  auto push_constant(VkShaderStageFlags stages, uint32 offset, uint32 size) -> Self&;

  [[nodiscard]] auto build() const -> VkPipelineLayout;

 private:
  Vector<VkDescriptorSetLayout> mDescriptorSetLayouts;
  Vector<VkPushConstantRange> mPushConstants;
};

class PipelineBuilder final {
  // The dynamic state is assumed to always be the viewport and scissor
  static constexpr VkDynamicState kDynamicStates[2] = {
      VK_DYNAMIC_STATE_VIEWPORT,
      VK_DYNAMIC_STATE_SCISSOR,
  };

 public:
  using Self = PipelineBuilder;

  explicit PipelineBuilder(VkPipelineCache cache = VK_NULL_HANDLE);

  auto reset() -> Self&;

  auto shaders(const char* vertex_path, const char* fragment_path) -> Self&;

  auto rasterization(VkPolygonMode polygon_mode, VkCullModeFlags cull_mode) -> Self&;

  auto multisample(VkSampleCountFlagBits samples) -> Self&;

  auto input_assembly(VkPrimitiveTopology topology) -> Self&;

  auto blending(bool enabled,
                VkBlendOp op = VK_BLEND_OP_ADD,
                VkBlendFactor src_factor = VK_BLEND_FACTOR_ONE,
                VkBlendFactor dst_factor = VK_BLEND_FACTOR_ZERO) -> Self&;

  [[nodiscard]] auto build(VkRenderPass pass, VkPipelineLayout layout) const
      -> VkPipeline;

 private:
  VkPipelineCache mCache;

  ShaderModule mVertexShader;
  ShaderModule mFragmentShader;
  VkPipelineShaderStageCreateInfo mShaderStages[2] {};

  // We assume that all pipelines use vertex shaders with the usual "Vertex" arguments
  Array<VkVertexInputBindingDescription, 1> mVertexInputBindings {};
  Array<VkVertexInputAttributeDescription, 3> mVertexAttributeDescriptions {};
  VkPipelineVertexInputStateCreateInfo mVertexInputState {};

  VkPipelineDynamicStateCreateInfo mDynamicState {};
  VkPipelineViewportStateCreateInfo mViewportState {};
  VkPipelineDepthStencilStateCreateInfo mDepthStencilState {};
  VkPipelineColorBlendAttachmentState mColorBlendAttachmentState {};

  Maybe<VkPipelineRasterizationStateCreateInfo> mRasterizationState;
  Maybe<VkPipelineMultisampleStateCreateInfo> mMultisampleState;
  Maybe<VkPipelineInputAssemblyStateCreateInfo> mInputAssemblyState;
  Maybe<VkPipelineColorBlendStateCreateInfo> mColorBlendState;
};

}  // namespace gravel::vk
