#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/type/path.hpp"

namespace gravel::vlk {

class ShaderModule final {
 public:
  GRAVEL_DELETE_COPY(ShaderModule);
  GRAVEL_DELETE_MOVE(ShaderModule);

  ShaderModule(VkDevice device, const Path& code_path);

  ~ShaderModule();

  [[nodiscard]] auto get() -> VkShaderModule { return mModule; }

 private:
  VkDevice mDevice {VK_NULL_HANDLE};
  VkShaderModule mModule {VK_NULL_HANDLE};
};

}  // namespace gravel::vlk
