#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "common/predef.hpp"

namespace gravel::vlk {

class Allocator final {
 public:
  GRAVEL_DELETE_COPY(Allocator);
  GRAVEL_DELETE_MOVE(Allocator);

  Allocator(VkInstance instance, VkPhysicalDevice gpu, VkDevice device);

  ~Allocator();

  [[nodiscard]] auto get() -> VmaAllocator { return mAllocator; }

 private:
  VmaAllocator mAllocator;
};

/// Context component for tracking the allocator to use when creating buffers, etc.
struct AllocatorContext final {
  VmaAllocator allocator {VK_NULL_HANDLE};
};

}  // namespace gravel::vlk
