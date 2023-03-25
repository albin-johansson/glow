#include "shading_pipeline.hpp"

#include <cstddef>  // offsetof

#include "common/debug/assert.hpp"
#include "common/debug/error.hpp"
#include "common/type/array.hpp"
#include "graphics/vertex.hpp"
#include "graphics/vulkan/shader_module.hpp"
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

  if (vkCreateDescriptorSetLayout(mDevice,
                                  &descriptor_set_layout_create_info,
                                  nullptr,
                                  &mDescriptorSetLayout) != VK_SUCCESS) {
    throw Error {"[VK] Could not create descriptor set layout"};
  }
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

  if (vkCreatePipelineLayout(mDevice, &layout_create_info, nullptr, &mPipelineLayout) !=
      VK_SUCCESS) {
    throw Error {"[VK] Could not create shading pipeline layout"};
  }
}

void ShadingPipeline::create_pipeline(VkRenderPass render_pass,
                                      const VkExtent2D swapchain_extent)
{
  GRAVEL_ASSERT(mDescriptorSetLayout != VK_NULL_HANDLE);
  GRAVEL_ASSERT(mPipelineLayout != VK_NULL_HANDLE);

  ShaderModule vertex_shader_module {mDevice, "assets/shaders/vk/shading.vert.spv"};
  ShaderModule fragment_shader_module {mDevice, "assets/shaders/vk/shading.frag.spv"};

  const VkPipelineShaderStageCreateInfo vertex_shader_stage_create_info {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,

      .stage = VK_SHADER_STAGE_VERTEX_BIT,
      .module = vertex_shader_module.get(),
      .pName = "main",
  };

  const VkPipelineShaderStageCreateInfo fragment_shader_stage_create_info {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,

      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = fragment_shader_module.get(),
      .pName = "main",
  };

  const VkDynamicState dynamic_states[] {
      VK_DYNAMIC_STATE_VIEWPORT,
      VK_DYNAMIC_STATE_SCISSOR,
  };

  const VkPipelineDynamicStateCreateInfo dynamic_state_create_info {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,

      .dynamicStateCount = array_length(dynamic_states),
      .pDynamicStates = dynamic_states,
  };

  // This describes the size and rate of the per-vertex attributes
  const VkVertexInputBindingDescription vertex_binding_descriptions[] {
      VkVertexInputBindingDescription {
          .binding = 0,
          .stride = sizeof(Vertex),
          .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
      },
  };

  // This describes the individual vertex attributes
  const VkVertexInputAttributeDescription vertex_attribute_descriptions[] {
      // vec3 position
      VkVertexInputAttributeDescription {
          .location = 0,
          .binding = 0,
          .format = VK_FORMAT_R32G32B32_SFLOAT,
          .offset = static_cast<uint32>(offsetof(Vertex, position)),
      },
      // vec3 normal
      VkVertexInputAttributeDescription {
          .location = 1,
          .binding = 0,
          .format = VK_FORMAT_R32G32B32_SFLOAT,
          .offset = static_cast<uint32>(offsetof(Vertex, normal)),
      },
      // vec2 tex_coords
      VkVertexInputAttributeDescription {
          .location = 2,
          .binding = 0,
          .format = VK_FORMAT_R32G32_SFLOAT,
          .offset = static_cast<uint32>(offsetof(Vertex, tex_coords)),
      },
  };

  const VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,

      .vertexBindingDescriptionCount = array_length(vertex_binding_descriptions),
      .pVertexBindingDescriptions = vertex_binding_descriptions,

      .vertexAttributeDescriptionCount = array_length(vertex_attribute_descriptions),
      .pVertexAttributeDescriptions = vertex_attribute_descriptions,
  };

  const VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,

      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      .primitiveRestartEnable = VK_FALSE,
  };

  const VkViewport viewport {
      .x = 0.0f,
      .y = 0.0f,

      .width = static_cast<float>(swapchain_extent.width),
      .height = static_cast<float>(swapchain_extent.height),

      .minDepth = 0.0f,
      .maxDepth = 1.0f,
  };

  const VkRect2D scissor {
      .offset = VkOffset2D {0, 0},
      .extent = swapchain_extent,
  };

  const VkPipelineViewportStateCreateInfo viewport_state_create_info {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,

      .viewportCount = 1,
      .pViewports = &viewport,
      .scissorCount = 1,
      .pScissors = &scissor,
  };

  const VkPipelineRasterizationStateCreateInfo rasterization_state_create_info {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,

      .depthClampEnable = VK_FALSE,
      .rasterizerDiscardEnable = VK_FALSE,

      .polygonMode = VK_POLYGON_MODE_FILL,
      .cullMode = VK_CULL_MODE_BACK_BIT,
      .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,

      .depthBiasEnable = VK_FALSE,
      .depthBiasConstantFactor = 0.0f,
      .depthBiasClamp = 0.0f,
      .depthBiasSlopeFactor = 0.0f,

      .lineWidth = 1.0f,
  };

  const VkPipelineMultisampleStateCreateInfo multisample_state_create_info {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,

      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
      .sampleShadingEnable = VK_FALSE,
      .minSampleShading = 1.0f,
      .pSampleMask = nullptr,
      .alphaToCoverageEnable = VK_FALSE,
      .alphaToOneEnable = VK_FALSE,
  };

  const VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,

      .depthCompareOp = VK_COMPARE_OP_LESS,
      .stencilTestEnable = VK_FALSE,
      .front = VkStencilOpState {VK_STENCIL_OP_KEEP},
      .minDepthBounds = 0.0f,
      .maxDepthBounds = 1.0f,
  };

  const VkPipelineColorBlendAttachmentState color_blend_attachment_state {
      .blendEnable = VK_FALSE,

      .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
      .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
      .colorBlendOp = VK_BLEND_OP_ADD,

      .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
      .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
      .alphaBlendOp = VK_BLEND_OP_ADD,

      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
  };

  const VkPipelineColorBlendStateCreateInfo color_blend_state_create_info {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,

      .logicOpEnable = VK_FALSE,
      .logicOp = VK_LOGIC_OP_COPY,

      .attachmentCount = 1,
      .pAttachments = &color_blend_attachment_state,

      .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f},
  };

  const VkPipelineShaderStageCreateInfo shader_stage_create_infos[] {
      vertex_shader_stage_create_info,
      fragment_shader_stage_create_info,
  };

  const VkGraphicsPipelineCreateInfo graphics_pipeline_create_info {
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,

      .stageCount = array_length(shader_stage_create_infos),
      .pStages = shader_stage_create_infos,

      .pVertexInputState = &vertex_input_state_create_info,
      .pInputAssemblyState = &input_assembly_state_create_info,
      .pViewportState = &viewport_state_create_info,
      .pRasterizationState = &rasterization_state_create_info,
      .pMultisampleState = &multisample_state_create_info,
      .pDepthStencilState = &depth_stencil_state_create_info,
      .pColorBlendState = &color_blend_state_create_info,
      .pDynamicState = &dynamic_state_create_info,

      .layout = mPipelineLayout,
      .renderPass = render_pass,
      .subpass = 0,

      .basePipelineHandle = VK_NULL_HANDLE,
      .basePipelineIndex = -1,
  };

  // TODO investigate VkPipelineCache
  if (vkCreateGraphicsPipelines(mDevice,
                                VK_NULL_HANDLE,
                                1,
                                &graphics_pipeline_create_info,
                                nullptr,
                                &mPipeline) != VK_SUCCESS) {
    throw Error {"[VK] Could not create shading pipeline"};
  }
}

ShadingPipeline::~ShadingPipeline()
{
  vkDestroyPipeline(mDevice, mPipeline, nullptr);
  vkDestroyPipelineLayout(mDevice, mPipelineLayout, nullptr);
  vkDestroyDescriptorSetLayout(mDevice, mDescriptorSetLayout, nullptr);
}

}  // namespace gravel::vlk
