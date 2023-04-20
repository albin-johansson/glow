#include "pipeline_builder.hpp"

#include "common/debug/error.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/pipeline/pipeline.hpp"
#include "graphics/vulkan/util/size.hpp"
#include "graphics/vulkan/util/vk_call.hpp"
#include "util/arrays.hpp"

namespace glow::vk {

auto DescriptorSetLayoutBuilder::reset() -> Self&
{
  mBindings.clear();
  mUsePushDescriptors = false;

  return *this;
}

auto DescriptorSetLayoutBuilder::use_push_descriptors() -> Self&
{
  mUsePushDescriptors = true;
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

auto DescriptorSetLayoutBuilder::build() const -> DescriptorSetLayoutPtr
{
  const Vector<VkDescriptorBindingFlags> binding_flags(
      mBindings.size(),
      VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);

  const VkDescriptorSetLayoutBindingFlagsCreateInfo binding_flags_create_info {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
      .pNext = nullptr,
      .bindingCount = u32_size(binding_flags),
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

      .bindingCount = u32_size(mBindings),
      .pBindings = mBindings.data(),
  };

  VkDescriptorSetLayout layout = VK_NULL_HANDLE;
  GLOW_VK_CALL(vkCreateDescriptorSetLayout(get_device(), &info, nullptr, &layout),
               "[VK] Could not create descriptor set layout");

  return DescriptorSetLayoutPtr {layout};
}

auto PipelineLayoutBuilder::reset() -> Self&
{
  mPushConstants.clear();
  mDescriptorSetLayouts.clear();
  return *this;
}

auto PipelineLayoutBuilder::descriptor_set_layout(VkDescriptorSetLayout set_layout)
    -> Self&
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

auto PipelineLayoutBuilder::build() const -> PipelineLayoutPtr
{
  const VkPipelineLayoutCreateInfo create_info {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,

      .setLayoutCount = u32_size(mDescriptorSetLayouts),
      .pSetLayouts = mDescriptorSetLayouts.data(),

      .pushConstantRangeCount = u32_size(mPushConstants),
      .pPushConstantRanges = mPushConstants.data(),
  };

  VkPipelineLayout layout = VK_NULL_HANDLE;
  GLOW_VK_CALL(vkCreatePipelineLayout(get_device(), &create_info, nullptr, &layout),
               "[VK] Could not create pipeline layout");

  return PipelineLayoutPtr {layout};
}

PipelineBuilder::PipelineBuilder(VkPipelineCache cache)
    : mCache {cache},
      mDynamicState {
          create_pipeline_dynamic_state(array_length(kDynamicStates), kDynamicStates)},
      mViewportState {create_pipeline_viewport_state()},
      mDepthStencilState {create_pipeline_depth_stencil_state()}
{
  reset();
}

auto PipelineBuilder::reset() -> Self&
{
  mRenderPass = VK_NULL_HANDLE;
  mLayout = VK_NULL_HANDLE;

  mVertexShader.reset();
  mFragmentShader.reset();

  mVertexInputBindings.clear();
  mVertexAttributeDescriptions.clear();

  return rasterization(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT)
      .multisample(VK_SAMPLE_COUNT_1_BIT)
      .input_assembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
      .blending(false);
}

auto PipelineBuilder::render_pass(VkRenderPass render_pass) -> Self&
{
  mRenderPass = render_pass;
  return *this;
}

auto PipelineBuilder::layout(VkPipelineLayout layout) -> Self&
{
  mLayout = layout;
  return *this;
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

auto PipelineBuilder::vertex_input_binding(const uint32 binding,
                                           const uint32 stride,
                                           const VkVertexInputRate rate) -> Self&
{
  mVertexInputBindings.push_back(VkVertexInputBindingDescription {
      .binding = binding,
      .stride = stride,
      .inputRate = rate,
  });

  return *this;
}

auto PipelineBuilder::vertex_attribute(const uint32 binding,
                                       const uint32 location,
                                       const VkFormat format,
                                       const uint32 offset) -> Self&
{
  mVertexAttributeDescriptions.push_back(VkVertexInputAttributeDescription {
      .location = location,
      .binding = binding,
      .format = format,
      .offset = offset,
  });

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
                               const VkBlendFactor dst_factor,
                               const VkBlendFactor src_alpha_factor,
                               const VkBlendFactor dst_alpha_factor) -> Self&
{
  mColorBlendAttachmentState =
      create_pipeline_color_blend_attachment_state(enabled,
                                                   op,
                                                   src_factor,
                                                   dst_factor,
                                                   src_alpha_factor,
                                                   dst_alpha_factor);
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

auto PipelineBuilder::build() const -> PipelinePtr
{
  if (!mRenderPass) {
    throw Error {"[VK] Missing pipeline render pass"};
  }
  else if (!mLayout) {
    throw Error {"[VK] Missing pipeline layout"};
  }
  else if (!mVertexShader || !mFragmentShader) {
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
  else if (mVertexInputBindings.empty()) {
    throw Error {"[VK] Missing vertex input bindings"};
  }
  else if (mVertexAttributeDescriptions.empty()) {
    throw Error {"[VK] Missing vertex input attribute descriptions"};
  }

  const auto& rasterization = mRasterizationState.value();
  const auto& multisample = mMultisampleState.value();
  const auto& input_assembly = mInputAssemblyState.value();
  const auto& color_blend = mColorBlendState.value();

  const VkPipelineVertexInputStateCreateInfo vertex_input_state {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,

      .pNext = nullptr,
      .flags = 0,

      .vertexBindingDescriptionCount = u32_size(mVertexInputBindings),
      .pVertexBindingDescriptions = mVertexInputBindings.data(),

      .vertexAttributeDescriptionCount = u32_size(mVertexAttributeDescriptions),
      .pVertexAttributeDescriptions = mVertexAttributeDescriptions.data(),
  };

  const VkGraphicsPipelineCreateInfo create_info {
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,

      .stageCount = 2,
      .pStages = mShaderStages,

      .pVertexInputState = &vertex_input_state,
      .pInputAssemblyState = &input_assembly,
      .pViewportState = &mViewportState,
      .pRasterizationState = &rasterization,
      .pMultisampleState = &multisample,
      .pDepthStencilState = &mDepthStencilState,
      .pColorBlendState = &color_blend,
      .pDynamicState = &mDynamicState,

      .layout = mLayout,
      .renderPass = mRenderPass,
      .subpass = 0,

      .basePipelineHandle = VK_NULL_HANDLE,
      .basePipelineIndex = -1,
  };

  VkPipeline pipeline = VK_NULL_HANDLE;
  GLOW_VK_CALL(vkCreateGraphicsPipelines(get_device(),
                                         mCache,
                                         1,
                                         &create_info,
                                         nullptr,
                                         &pipeline),
               "[VK] Could not create pipeline");

  return PipelinePtr {pipeline};
}

}  // namespace glow::vk
