#pragma once

#include <vulkan/vulkan.h>

namespace gravel::vlk {

[[nodiscard]] auto create_semaphore(VkDevice device) -> VkSemaphore;

[[nodiscard]] auto create_signaled_fence(VkDevice device) -> VkFence;

}  // namespace gravel::vlk
