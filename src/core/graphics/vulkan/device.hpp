#pragma once

#include <vulkan/vulkan.h>

#include "common/type/memory.hpp"

namespace glow::vk {

struct DeviceDeleter final {
  void operator()(VkDevice device) noexcept;
};

using Device = Unique<VkDevice_T, DeviceDeleter>;

[[nodiscard]] auto create_device() -> Device;

}  // namespace glow::vk
