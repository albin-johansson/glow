#pragma once

#include <vulkan/vulkan.h>

#include "common/type/memory.hpp"

namespace glow::vk {

struct SemaphoreDeleter final {
  void operator()(VkSemaphore semaphore) noexcept;
};

/// Represents a binary semaphore, for GPU-side synchronization.
using SemaphorePtr = Unique<VkSemaphore_T, SemaphoreDeleter>;

[[nodiscard]] auto create_semaphore() -> SemaphorePtr;

}  // namespace glow::vk
