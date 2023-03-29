#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"

namespace gravel::vlk {

/// Represents a binary semaphore, for CPU-side synchronization (known as a "fence").
class Fence final {
 public:
  GRAVEL_DELETE_COPY(Fence);

  explicit Fence(bool signaled = true);

  ~Fence() noexcept;

  Fence(Fence&& other) noexcept;

  auto operator=(Fence&& other) noexcept -> Fence&;

  /// Waits on the fence to get signaled.
  void wait();

  /// Resets the fence to its unsignaled state.
  void reset();

  [[nodiscard]] auto get() -> VkFence { return mFence; }

 private:
  VkFence mFence {VK_NULL_HANDLE};

  void dispose() noexcept;
};

}  // namespace gravel::vlk
