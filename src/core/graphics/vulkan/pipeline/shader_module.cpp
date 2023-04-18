#include "shader_module.hpp"

#include "common/debug/error.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util/vk_call.hpp"
#include "io/files.hpp"

namespace glow::vk {

void ShaderModuleDeleter::operator()(VkShaderModule shader) noexcept
{
  vkDestroyShaderModule(get_device(), shader, nullptr);
}

auto create_shader_module(const Path& shader_path) -> ShaderModulePtr
{
  const auto code = load_binary_file(shader_path);
  if (!code) {
    throw Error {"[VK] Could not load shader code"};
  }

  const VkShaderModuleCreateInfo create_info {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .codeSize = code->size(),
      .pCode = reinterpret_cast<const uint32*>(code->data()),  // NOLINT
  };

  VkShaderModule shader = VK_NULL_HANDLE;
  GLOW_VK_CALL(vkCreateShaderModule(get_device(), &create_info, nullptr, &shader),
               "[VK] Could not create shader module");

  return ShaderModulePtr {shader};
}

}  // namespace glow::vk
