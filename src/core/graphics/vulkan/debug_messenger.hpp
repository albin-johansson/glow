#pragma once

#include <vulkan/vulkan.h>

#include "common/type/memory.hpp"

namespace gravel::vk {

struct DebugMessengerDeleter final {
  void operator()(VkDebugUtilsMessengerEXT messenger) noexcept;
};

using DebugMessenger = Unique<VkDebugUtilsMessengerEXT_T, DebugMessengerDeleter>;

[[nodiscard]] auto create_debug_messenger() -> DebugMessenger;

}  // namespace gravel::vk
