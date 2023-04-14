#include "pipeline.hpp"

#include <cstddef>  // offsetof

#include "graphics/vertex.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util/vk_call.hpp"

namespace gravel::vk {

void PipelineDeleter::operator()(VkPipeline pipeline) noexcept
{
  vkDestroyPipeline(get_device(), pipeline, nullptr);
}

auto create_pipeline_cache(VkDevice device) -> VkPipelineCache
{
  const VkPipelineCacheCreateInfo create_info {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .initialDataSize = 0,
      .pInitialData = nullptr,
  };

  VkPipelineCache cache = VK_NULL_HANDLE;
  GRAVEL_VK_CALL(vkCreatePipelineCache(device, &create_info, nullptr, &cache),
                 "[VK] Could not create pipeline cache");

  return cache;
}

auto create_pipeline_shader_stage(const VkShaderStageFlagBits stage,
                                  VkShaderModule shader_module)
    -> VkPipelineShaderStageCreateInfo
{
  const VkPipelineShaderStageCreateInfo shader_stage {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,

      .pNext = nullptr,
      .flags = 0,

      .stage = stage,
      .module = shader_module,
      .pName = "main",

      .pSpecializationInfo = nullptr,
  };

  return shader_stage;
}

auto create_pipeline_dynamic_state(const uint32 state_count, const VkDynamicState* states)
    -> VkPipelineDynamicStateCreateInfo
{
  const VkPipelineDynamicStateCreateInfo dynamic_states {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,

      .pNext = nullptr,
      .flags = 0,

      .dynamicStateCount = state_count,
      .pDynamicStates = states,
  };

  return dynamic_states;
}

auto create_pipeline_rasterization_state(const VkPolygonMode polygon_mode,
                                         const VkCullModeFlags cull_mode)
    -> VkPipelineRasterizationStateCreateInfo
{
  const VkPipelineRasterizationStateCreateInfo rasterization_state {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,

      .pNext = nullptr,
      .flags = 0,

      .depthClampEnable = VK_FALSE,
      .rasterizerDiscardEnable = VK_FALSE,

      .polygonMode = polygon_mode,
      .cullMode = cull_mode,
      .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,

      .depthBiasEnable = VK_FALSE,
      .depthBiasConstantFactor = 0.0f,
      .depthBiasClamp = 0.0f,
      .depthBiasSlopeFactor = 0.0f,

      .lineWidth = 1.0f,
  };

  return rasterization_state;
}

auto create_pipeline_multisample_state(const VkSampleCountFlagBits samples)
    -> VkPipelineMultisampleStateCreateInfo
{
  const VkPipelineMultisampleStateCreateInfo multisample_state {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,

      .pNext = nullptr,
      .flags = 0,

      .rasterizationSamples = samples,
      .sampleShadingEnable = VK_FALSE,
      .minSampleShading = 1.0f,
      .pSampleMask = nullptr,
      .alphaToCoverageEnable = VK_FALSE,
      .alphaToOneEnable = VK_FALSE,
  };

  return multisample_state;
}

auto create_pipeline_viewport_state() -> VkPipelineViewportStateCreateInfo
{
  const VkPipelineViewportStateCreateInfo viewport_state {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,

      .pNext = nullptr,
      .flags = 0,

      .viewportCount = 1,
      .pViewports = nullptr,

      .scissorCount = 1,
      .pScissors = nullptr,
  };

  return viewport_state;
}

auto create_pipeline_input_assembly_state(const VkPrimitiveTopology topology)
    -> VkPipelineInputAssemblyStateCreateInfo
{
  const VkPipelineInputAssemblyStateCreateInfo input_assembly_state {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,

      .pNext = nullptr,
      .flags = 0,

      .topology = topology,
      .primitiveRestartEnable = VK_FALSE,
  };

  return input_assembly_state;
}

auto create_pipeline_depth_stencil_state() -> VkPipelineDepthStencilStateCreateInfo
{
  const VkPipelineDepthStencilStateCreateInfo depth_stencil_state {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,

      .pNext = nullptr,
      .flags = 0,

      .depthTestEnable = VK_TRUE,
      .depthWriteEnable = VK_TRUE,
      .depthCompareOp = VK_COMPARE_OP_LESS,
      .depthBoundsTestEnable = VK_FALSE,

      .stencilTestEnable = VK_FALSE,
      .front = VkStencilOpState {},
      .back = VkStencilOpState {},

      .minDepthBounds = 0.0f,
      .maxDepthBounds = 1.0f,
  };

  return depth_stencil_state;
}

auto create_pipeline_color_blend_attachment_state(const bool enabled,
                                                  const VkBlendOp op,
                                                  const VkBlendFactor src_factor,
                                                  const VkBlendFactor dst_factor,
                                                  const VkBlendFactor src_alpha_factor,
                                                  const VkBlendFactor dst_alpha_factor)
    -> VkPipelineColorBlendAttachmentState
{
  const VkPipelineColorBlendAttachmentState color_blend_attachment_state {
      .blendEnable = enabled ? VK_TRUE : VK_FALSE,

      .srcColorBlendFactor = src_factor,
      .dstColorBlendFactor = dst_factor,
      .colorBlendOp = op,

      .srcAlphaBlendFactor = src_alpha_factor,
      .dstAlphaBlendFactor = dst_alpha_factor,
      .alphaBlendOp = op,

      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
  };

  return color_blend_attachment_state;
}

auto create_vertex_binding_descriptions(const uint32 binding)
    -> Array<VkVertexInputBindingDescription, 1>
{
  // This describes the size and rate of the per-vertex attributes
  const std::array vertex_binding_descriptions {
      VkVertexInputBindingDescription {
          .binding = binding,
          .stride = sizeof(Vertex),
          .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
      },
  };

  return vertex_binding_descriptions;
}

auto create_vertex_attribute_descriptions() -> Array<VkVertexInputAttributeDescription, 3>
{
  const std::array vertex_attribute_descriptions {
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

  return vertex_attribute_descriptions;
}

}  // namespace gravel::vk
