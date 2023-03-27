#include "shading_pipeline.hpp"

#include "common/debug/assert.hpp"
#include "common/debug/error.hpp"
#include "graphics/vertex.hpp"
#include "graphics/vulkan/pipeline.hpp"
#include "graphics/vulkan/shader_module.hpp"
#include "graphics/vulkan/util.hpp"
#include "util/arrays.hpp"

namespace gravel::vlk {

ShadingPipeline::ShadingPipeline(VkDevice device,
                                 VkRenderPass render_pass,
                                 const VkExtent2D swapchain_extent)
    : mDevice {device}
{
  create_descriptor_set_layout();
  create_pipeline_layout();
  create_pipeline(render_pass, swapchain_extent);
}

void ShadingPipeline::create_descriptor_set_layout()
{
  const VkDescriptorSetLayoutBinding descriptor_set_layout_bindings[] {
      // uniform MatrixBuffer
      VkDescriptorSetLayoutBinding {
          .binding = 0,
          .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
          .descriptorCount = 1,
          .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
          .pImmutableSamplers = nullptr,
      },
      // uniform MaterialBuffer
      VkDescriptorSetLayoutBinding {
          .binding = 1,
          .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
          .descriptorCount = 1,
          .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
          .pImmutableSamplers = nullptr,
      },
      // sampler2D material_diffuse_tex
      VkDescriptorSetLayoutBinding {
          .binding = 5,
          .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
          .descriptorCount = 1,
          .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
          .pImmutableSamplers = nullptr,
      },
  };

  const VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,

      .bindingCount = array_length(descriptor_set_layout_bindings),
      .pBindings = descriptor_set_layout_bindings,
  };

  GRAVEL_VK_CALL(vkCreateDescriptorSetLayout(mDevice,
                                             &descriptor_set_layout_create_info,
                                             nullptr,
                                             &mDescriptorSetLayout),
                 "[VK] Could not create descriptor set layout");
}

void ShadingPipeline::create_pipeline_layout()
{
  GRAVEL_ASSERT(mDescriptorSetLayout != VK_NULL_HANDLE);

  const VkPipelineLayoutCreateInfo layout_create_info {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,

      .setLayoutCount = 1,
      .pSetLayouts = &mDescriptorSetLayout,

      .pushConstantRangeCount = 0,
      .pPushConstantRanges = nullptr,
  };

  GRAVEL_VK_CALL(vkCreatePipelineLayout(mDevice,  //
                                        &layout_create_info,
                                        nullptr,
                                        &mPipelineLayout),
                 "[VK] Could not create shading pipeline layout");
}

void ShadingPipeline::create_pipeline(VkRenderPass render_pass,
                                      const VkExtent2D swapchain_extent)
{
  GRAVEL_ASSERT(mDescriptorSetLayout != VK_NULL_HANDLE);
  GRAVEL_ASSERT(mPipelineLayout != VK_NULL_HANDLE);

  ShaderModule vertex_shader {mDevice, "assets/shaders/vk/shading.vert.spv"};
  ShaderModule fragment_shader {mDevice, "assets/shaders/vk/shading.frag.spv"};

  const VkPipelineShaderStageCreateInfo shader_stages[] {
      create_pipeline_shader_stage(VK_SHADER_STAGE_VERTEX_BIT, vertex_shader.get()),
      create_pipeline_shader_stage(VK_SHADER_STAGE_FRAGMENT_BIT, fragment_shader.get()),
  };

  const VkDynamicState dynamic_states[] {
      VK_DYNAMIC_STATE_VIEWPORT,
      VK_DYNAMIC_STATE_SCISSOR,
  };

  const auto dynamic_state =
      create_pipeline_dynamic_state(array_length(dynamic_states), dynamic_states);

  const auto vertex_binding_descriptions = create_vertex_binding_descriptions(0);
  const auto vertex_attribute_descriptions = create_vertex_attribute_descriptions();

  const VkPipelineVertexInputStateCreateInfo vertex_input_state {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,

      .pNext = nullptr,
      .flags = 0,

      .vertexBindingDescriptionCount = vertex_binding_descriptions.size(),
      .pVertexBindingDescriptions = vertex_binding_descriptions.data(),

      .vertexAttributeDescriptionCount = vertex_attribute_descriptions.size(),
      .pVertexAttributeDescriptions = vertex_attribute_descriptions.data(),
  };

  const auto input_assembly_state = create_pipeline_input_assembly_state();
  const auto viewport_state = create_pipeline_viewport_state();
  const auto rasterization_state =
      create_pipeline_rasterization_state(VK_POLYGON_MODE_FILL);
  const auto multisample_state = create_pipeline_multisample_state(VK_SAMPLE_COUNT_1_BIT);
  const auto depth_stencil_state = create_pipeline_depth_stencil_state();

  const auto color_blend_attachment_state =
      create_pipeline_color_blend_attachment_state(false);

  const VkPipelineColorBlendStateCreateInfo color_blend_state_create_info {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,

      .logicOpEnable = VK_FALSE,
      .logicOp = VK_LOGIC_OP_COPY,

      .attachmentCount = 1,
      .pAttachments = &color_blend_attachment_state,

      .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f},
  };

  const VkGraphicsPipelineCreateInfo graphics_pipeline_create_info {
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,

      .stageCount = array_length(shader_stages),
      .pStages = shader_stages,

      .pVertexInputState = &vertex_input_state,
      .pInputAssemblyState = &input_assembly_state,
      .pViewportState = &viewport_state,
      .pRasterizationState = &rasterization_state,
      .pMultisampleState = &multisample_state,
      .pDepthStencilState = &depth_stencil_state,
      .pColorBlendState = &color_blend_state_create_info,
      .pDynamicState = &dynamic_state,

      .layout = mPipelineLayout,
      .renderPass = render_pass,
      .subpass = 0,

      .basePipelineHandle = VK_NULL_HANDLE,
      .basePipelineIndex = -1,
  };

  // TODO investigate VkPipelineCache
  GRAVEL_VK_CALL(vkCreateGraphicsPipelines(mDevice,
                                           VK_NULL_HANDLE,
                                           1,
                                           &graphics_pipeline_create_info,
                                           nullptr,
                                           &mPipeline),
                 "[VK] Could not create shading pipeline");
}

ShadingPipeline::~ShadingPipeline()
{
  vkDestroyPipeline(mDevice, mPipeline, nullptr);
  vkDestroyPipelineLayout(mDevice, mPipelineLayout, nullptr);
  vkDestroyDescriptorSetLayout(mDevice, mDescriptorSetLayout, nullptr);
}

}  // namespace gravel::vlk
