#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "common/type/memory.hpp"

namespace gravel::vk {

struct AllocatorDeleter final {
  void operator()(VmaAllocator allocator) noexcept;
};

/// Represents a dedicated Vulkan memory allocator.
using Allocator = Unique<VmaAllocator_T, AllocatorDeleter>;

[[nodiscard]] auto create_allocator() -> Allocator;

}  // namespace gravel::vk
