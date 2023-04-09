#pragma once

#include <vulkan/vulkan.h>

#include "common/type/memory.hpp"

namespace gravel::vlk {

struct SemaphoreDeleter final {
  void operator()(VkSemaphore semaphore) noexcept;
};

/// Represents a binary semaphore, for GPU-side synchronization.
using Semaphore = Unique<VkSemaphore_T, SemaphoreDeleter>;

[[nodiscard]] auto create_semaphore() -> Semaphore;

}  // namespace gravel::vlk
