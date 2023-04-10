#pragma once

#include <vulkan/vulkan.h>

#include "common/type/memory.hpp"

namespace gravel::vk {

struct FenceDeleter final {
  void operator()(VkFence fence) noexcept;
};

/// Represents a binary semaphore, for CPU-side synchronization (known as a "fence").
using Fence = Unique<VkFence_T, FenceDeleter>;

[[nodiscard]] auto create_fence(bool signaled = true) -> Fence;

/// Resets a fence to its unsignaled state.
void reset_fence(VkFence fence);

/// Blocks the calling thread until the fence gets signaled.
void wait_fence(VkFence fence);

}  // namespace gravel::vk
