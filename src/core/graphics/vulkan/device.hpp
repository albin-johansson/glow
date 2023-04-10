#pragma once

#include <vulkan/vulkan.h>

#include "common/type/memory.hpp"

namespace gravel::vlk {

struct DeviceDeleter final {
  void operator()(VkDevice device) noexcept;
};

using Device = Unique<VkDevice_T, DeviceDeleter>;

[[nodiscard]] auto create_device() -> Device;

}  // namespace gravel::vlk
