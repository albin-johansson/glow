#pragma once

#include <vulkan/vulkan.h>

namespace gravel::vlk {

/// Waits on a fence to get signaled, and subsequently reset it.
void wait_and_then_reset_fence(VkDevice device, VkFence fence);

/// Creates a binary semaphore, for GPU-side synchronization.
[[nodiscard]] auto create_semaphore(VkDevice device) -> VkSemaphore;

/// Creates a binary semaphore, for CPU-side synchronization (known as a "fence").
[[nodiscard]] auto create_signaled_fence(VkDevice device) -> VkFence;

}  // namespace gravel::vlk
