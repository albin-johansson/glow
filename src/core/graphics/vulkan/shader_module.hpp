#pragma once

#include <vulkan/vulkan.h>

#include "common/type/memory.hpp"
#include "common/type/path.hpp"

namespace gravel::vk {

struct ShaderModuleDeleter final {
  void operator()(VkShaderModule shader) noexcept;
};

using ShaderModule = Unique<VkShaderModule_T, ShaderModuleDeleter>;

[[nodiscard]] auto create_shader_module(const Path& shader_path) -> ShaderModule;

}  // namespace gravel::vk
