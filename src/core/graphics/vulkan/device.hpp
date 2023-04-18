#pragma once

#include <vulkan/vulkan.h>

#include "common/type/memory.hpp"

namespace glow::vk {

struct DeviceDeleter final {
  void operator()(VkDevice device) noexcept;
};

using DevicePtr = Unique<VkDevice_T, DeviceDeleter>;

[[nodiscard]] auto create_device() -> DevicePtr;

}  // namespace glow::vk
