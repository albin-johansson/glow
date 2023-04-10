#pragma once

#include <vulkan/vulkan.h>

#include "common/type/memory.hpp"

namespace gravel::vk {

struct InstanceDeleter final {
  void operator()(VkInstance instance) noexcept;
};

using Instance = Unique<VkInstance_T, InstanceDeleter>;

[[nodiscard]] auto create_instance() -> Instance;

}  // namespace gravel::vk
