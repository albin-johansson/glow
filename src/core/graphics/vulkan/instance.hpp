#pragma once

#include <vulkan/vulkan.h>

#include "common/type/memory.hpp"

namespace glow::vk {

struct InstanceDeleter final {
  void operator()(VkInstance instance) noexcept;
};

using InstancePtr = Unique<VkInstance_T, InstanceDeleter>;

[[nodiscard]] auto create_instance() -> InstancePtr;

}  // namespace glow::vk
