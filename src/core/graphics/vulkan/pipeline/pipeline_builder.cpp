#include "pipeline_builder.hpp"

#include "common/debug/error.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/pipeline/pipeline.hpp"
#include "graphics/vulkan/util.hpp"
#include "util/arrays.hpp"

namespace gravel::vk {

auto DescriptorSetLayoutBuilder::reset() -> Self&
{
  mBindings.clear();
  mUsePushDescriptors = false;

  return *this;
}

auto DescriptorSetLayoutBuilder::descriptor(const uint32 binding,
                                            const VkDescriptorType type,
                                            const VkShaderStageFlags stages,
                                            const uint32 count) -> Self&
{
  mBindings.push_back(VkDescriptorSetLayoutBinding {
      .binding = binding,
      .descriptorType = type,
      .descriptorCount = count,
      .stageFlags = stages,
      .pImmutableSamplers = nullptr,
  });

  return *this;
}

auto DescriptorSetLayoutBuilder::use_push_descriptors() -> Self&
{
  mUsePushDescriptors = true;
  return *this;
}

auto DescriptorSetLayoutBuilder::build() const -> VkDescriptorSetLayout
{
  const Vector<VkDescriptorBindingFlags> binding_flags(
      mBindings.size(),
      VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);

  const VkDescriptorSetLayoutBindingFlagsCreateInfo binding_flags_create_info {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
      .pNext = nullptr,
      .bindingCount = static_cast<uint32>(binding_flags.size()),
      .pBindingFlags = binding_flags.data(),
  };

  VkDescriptorSetLayoutCreateFlags flags = 0;
  if (mUsePushDescriptors) {
    flags |= VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR;
  }

  const VkDescriptorSetLayoutCreateInfo info {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,

      .pNext = &binding_flags_create_info,
      .flags = flags,

      .bindingCount = static_cast<uint32>(mBindings.size()),
      .pBindings = mBindings.data(),
  };

  VkDescriptorSetLayout layout = VK_NULL_HANDLE;
  GRAVEL_VK_CALL(vkCreateDescriptorSetLayout(get_device(), &info, nullptr, &layout),
                 "[VK] Could not create descriptor set layout");

  return layout;
}

auto PipelineLayoutBuilder::reset() -> Self&
{
  mPushConstants.clear();
  mDescriptorSetLayouts.clear();
  return *this;
}

auto PipelineLayoutBuilder::descriptor_layout(VkDescriptorSetLayout set_layout) -> Self&
{
  mDescriptorSetLayouts.push_back(set_layout);
  return *this;
}

auto PipelineLayoutBuilder::push_constant(const VkShaderStageFlags stages,
                                          const uint32 offset,
                                          const uint32 size) -> Self&
{
  mPushConstants.push_back(VkPushConstantRange {
      .stageFlags = stages,
      .offset = offset,
      .size = size,
  });
  return *this;
}

auto PipelineLayoutBuilder::build() const -> VkPipelineLayout
{
  const VkPipelineLayoutCreateInfo create_info {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,

      .setLayoutCount = static_cast<uint32>(mDescriptorSetLayouts.size()),
      .pSetLayouts = mDescriptorSetLayouts.data(),

      .pushConstantRangeCount = static_cast<uint32>(mPushConstants.size()),
      .pPushConstantRanges = mPushConstants.data(),
  };

  VkPipelineLayout layout = VK_NULL_HANDLE;
  GRAVEL_VK_CALL(vkCreatePipelineLayout(get_device(), &create_info, nullptr, &layout),
                 "[VK] Could not create pipeline layout");

  return layout;
}

PipelineBuilder::PipelineBuilder(VkPipelineCache cache)
    : mCache {cache},
      mVertexInputBindings {create_vertex_binding_descriptions(0)},
      mVertexAttributeDescriptions {create_vertex_attribute_descriptions()},
      mDynamicState {
          create_pipeline_dynamic_state(array_length(kDynamicStates), kDynamicStates)},
      mViewportState {create_pipeline_viewport_state()},
      mDepthStencilState {create_pipeline_depth_stencil_state()}
{
  mVertexInputState = VkPipelineVertexInputStateCreateInfo {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,

      .pNext = nullptr,
      .flags = 0,

      .vertexBindingDescriptionCount = static_cast<uint32>(mVertexInputBindings.size()),
      .pVertexBindingDescriptions = mVertexInputBindings.data(),

      .vertexAttributeDescriptionCount =
          static_cast<uint32>(mVertexAttributeDescriptions.size()),
      .pVertexAttributeDescriptions = mVertexAttributeDescriptions.data(),
  };

  reset();
}

auto PipelineBuilder::reset() -> Self&
{
  mVertexShader.reset();
  mFragmentShader.reset();

  return rasterization(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT)
      .multisample(VK_SAMPLE_COUNT_1_BIT)
      .input_assembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
      .blending(false);
}

auto PipelineBuilder::shaders(const char* vertex_path, const char* fragment_path) -> Self&
{
  mVertexShader = create_shader_module(vertex_path);
  mFragmentShader = create_shader_module(fragment_path);

  mShaderStages[0] =
      create_pipeline_shader_stage(VK_SHADER_STAGE_VERTEX_BIT, mVertexShader.get());
  mShaderStages[1] =
      create_pipeline_shader_stage(VK_SHADER_STAGE_FRAGMENT_BIT, mFragmentShader.get());

  return *this;
}

auto PipelineBuilder::rasterization(const VkPolygonMode polygon_mode,
                                    const VkCullModeFlags cull_mode) -> Self&
{
  mRasterizationState = create_pipeline_rasterization_state(polygon_mode, cull_mode);
  return *this;
}

auto PipelineBuilder::multisample(const VkSampleCountFlagBits samples) -> Self&
{
  mMultisampleState = create_pipeline_multisample_state(samples);
  return *this;
}

auto PipelineBuilder::input_assembly(const VkPrimitiveTopology topology) -> Self&
{
  mInputAssemblyState = create_pipeline_input_assembly_state(topology);
  return *this;
}

auto PipelineBuilder::blending(const bool enabled,
                               const VkBlendOp op,
                               const VkBlendFactor src_factor,
                               const VkBlendFactor dst_factor) -> Self&
{
  mColorBlendAttachmentState =
      create_pipeline_color_blend_attachment_state(enabled, op, src_factor, dst_factor);
  mColorBlendState = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,

      .logicOpEnable = VK_FALSE,
      .logicOp = VK_LOGIC_OP_COPY,

      .attachmentCount = 1,
      .pAttachments = &mColorBlendAttachmentState,

      .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f},
  };

  return *this;
}

auto PipelineBuilder::build(VkRenderPass pass, VkPipelineLayout layout) const
    -> VkPipeline
{
  if (!mVertexShader || !mFragmentShader) {
    throw Error {"[VK] Missing pipeline shaders"};
  }
  else if (!mRasterizationState) {
    throw Error {"[VK] Missing pipeline rasterization state"};
  }
  else if (!mInputAssemblyState) {
    throw Error {"[VK] Missing pipeline input assembly state"};
  }
  else if (!mColorBlendState) {
    throw Error {"[VK] Missing pipeline color blend state"};
  }

  auto& rasterization = mRasterizationState.value();
  auto& multisample = mMultisampleState.value();
  auto& input_assembly = mInputAssemblyState.value();
  auto& color_blend = mColorBlendState.value();

  const VkGraphicsPipelineCreateInfo create_info {
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,

      .stageCount = 2,
      .pStages = mShaderStages,

      .pVertexInputState = &mVertexInputState,
      .pInputAssemblyState = &input_assembly,
      .pViewportState = &mViewportState,
      .pRasterizationState = &rasterization,
      .pMultisampleState = &multisample,
      .pDepthStencilState = &mDepthStencilState,
      .pColorBlendState = &color_blend,
      .pDynamicState = &mDynamicState,

      .layout = layout,
      .renderPass = pass,
      .subpass = 0,

      .basePipelineHandle = VK_NULL_HANDLE,
      .basePipelineIndex = -1,
  };

  VkPipeline pipeline = VK_NULL_HANDLE;
  GRAVEL_VK_CALL(vkCreateGraphicsPipelines(get_device(),
                                           mCache,
                                           1,
                                           &create_info,
                                           nullptr,
                                           &pipeline),
                 "[VK] Could not create pipeline");

  return pipeline;
}

}  // namespace gravel::vk
