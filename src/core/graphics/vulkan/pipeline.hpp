#pragma once

#include <vulkan/vulkan.h>

#include "common/primitives.hpp"
#include "common/type/array.hpp"

namespace gravel::vlk {

[[nodiscard]] auto create_pipeline_cache(VkDevice device) -> VkPipelineCache;

[[nodiscard]] auto create_pipeline_shader_stage(VkShaderStageFlagBits stage,
                                                VkShaderModule shader_module)
    -> VkPipelineShaderStageCreateInfo;

[[nodiscard]] auto create_pipeline_dynamic_state(uint32 state_count,
                                                 const VkDynamicState* states)
    -> VkPipelineDynamicStateCreateInfo;

[[nodiscard]] auto create_pipeline_rasterization_state(VkPolygonMode polygon_mode,
                                                       VkCullModeFlags cull_mode)
    -> VkPipelineRasterizationStateCreateInfo;

[[nodiscard]] auto create_pipeline_multisample_state(VkSampleCountFlagBits samples)
    -> VkPipelineMultisampleStateCreateInfo;

[[nodiscard]] auto create_pipeline_viewport_state() -> VkPipelineViewportStateCreateInfo;

[[nodiscard]] auto create_pipeline_input_assembly_state(VkPrimitiveTopology topology)
    -> VkPipelineInputAssemblyStateCreateInfo;

[[nodiscard]] auto create_pipeline_depth_stencil_state()
    -> VkPipelineDepthStencilStateCreateInfo;

[[nodiscard]] auto create_pipeline_color_blend_attachment_state(bool enabled,
                                                                VkBlendOp op,
                                                                VkBlendFactor src_factor,
                                                                VkBlendFactor dst_factor)
    -> VkPipelineColorBlendAttachmentState;

/// Returns an array of vertex binding descriptions for the usual Vertex type.
[[nodiscard]] auto create_vertex_binding_descriptions(uint32 binding)
    -> Array<VkVertexInputBindingDescription, 1>;

/// Returns an array of vertex attribute descriptions for the usual Vertex type.
[[nodiscard]] auto create_vertex_attribute_descriptions()
    -> Array<VkVertexInputAttributeDescription, 3>;

}  // namespace gravel::vlk
