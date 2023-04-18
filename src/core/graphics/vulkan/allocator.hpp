#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "common/type/memory.hpp"

namespace glow::vk {

struct AllocatorDeleter final {
  void operator()(VmaAllocator allocator) noexcept;
};

/// Represents a dedicated Vulkan memory allocator.
using AllocatorPtr = Unique<VmaAllocator_T, AllocatorDeleter>;

[[nodiscard]] auto create_allocator() -> AllocatorPtr;

}  // namespace glow::vk
