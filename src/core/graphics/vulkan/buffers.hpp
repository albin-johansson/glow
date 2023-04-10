#pragma once

#include <vulkan/vulkan.h>

#include "common/primitives.hpp"
#include "common/type/math.hpp"

namespace gravel::vk {

/// Holds matrices that remain constant over a single frame.
/// \note this struct assumes the std140 layout!
struct StaticMatrices final {
  Mat4 proj {};       ///< Projection matrix.
  Mat4 view {};       ///< View matrix.
  Mat4 view_proj {};  ///< View-projection matrix.

  [[nodiscard]] static auto descriptor_buffer_info(VkBuffer buffer)
      -> VkDescriptorBufferInfo;

  [[nodiscard]] static auto descriptor_set_write(VkDescriptorSet set,
                                                 const VkDescriptorBufferInfo* info)
      -> VkWriteDescriptorSet;
};

/// Intended to be used as a push constant for per model mesh matrices.
/// \note This struct assumes the std140 layout!
struct DynamicMatrices final {
  Mat4 model {};       ///< Model matrix.
  Mat4 model_view {};  ///< Model-view matrix.
  Mat4 mvp {};         ///< Model-view-projection matrix.
  Mat4 normal {};      ///< Normal matrix.

  [[nodiscard]] static auto push_constant_range(uint32 offset, VkShaderStageFlags stages)
      -> VkPushConstantRange;
};

/// This struct corresponds to a std140 layout uniform block.
struct MaterialBuffer final {
  alignas(16) Vec4 ambient {};
  alignas(16) Vec4 diffuse {};
  alignas(16) Vec4 specular {};
  alignas(16) Vec4 emission {};
  int32 has_ambient_tex {false};
  int32 has_diffuse_tex {false};
  int32 has_specular_tex {false};
  int32 has_emission_tex {false};
};

}  // namespace gravel::vk