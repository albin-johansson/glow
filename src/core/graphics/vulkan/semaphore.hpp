#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"

namespace gravel::vlk {

/// Represents a binary semaphore, for GPU-side synchronization.
class Semaphore final {
 public:
  GRAVEL_DELETE_COPY(Semaphore);

  Semaphore();

  ~Semaphore() noexcept;

  Semaphore(Semaphore&& other) noexcept;

  auto operator=(Semaphore&& other) noexcept -> Semaphore&;

  [[nodiscard]] auto get() -> VkSemaphore { return mSemaphore; }

 private:
  VkSemaphore mSemaphore {VK_NULL_HANDLE};

  void dispose() noexcept;
};

}  // namespace gravel::vlk
