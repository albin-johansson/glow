#pragma once

#include <vulkan/vulkan.h>

#include "common/type/memory.hpp"
#include "common/type/path.hpp"

namespace glow::vk {

struct ShaderModuleDeleter final {
  void operator()(VkShaderModule shader) noexcept;
};

using ShaderModulePtr = Unique<VkShaderModule_T, ShaderModuleDeleter>;

[[nodiscard]] auto create_shader_module(const Path& shader_path) -> ShaderModulePtr;

}  // namespace glow::vk
