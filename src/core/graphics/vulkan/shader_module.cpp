#include "shader_module.hpp"

#include "common/debug/error.hpp"
#include "common/primitives.hpp"
#include "graphics/vulkan/util.hpp"
#include "io/files.hpp"

namespace gravel::vlk {

ShaderModule::ShaderModule(VkDevice device, const Path& code_path)
    : mDevice {device}
{
  const auto code = load_binary_file(code_path);
  if (!code) {
    throw Error {"[VK] Could not load shader code"};
  }

  const VkShaderModuleCreateInfo shader_module_create_info {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .codeSize = code->size(),
      .pCode = reinterpret_cast<const uint32*>(code->data()),
  };

  GRAVEL_VK_CALL(vkCreateShaderModule(mDevice,  //
                                      &shader_module_create_info,
                                      nullptr,
                                      &mModule),
                 "[VK] Could not create shader module");
}

ShaderModule::~ShaderModule()
{
  vkDestroyShaderModule(mDevice, mModule, nullptr);
}

}  // namespace gravel::vlk
