#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"

namespace gravel::vlk {

class Buffer final {
 public:
  GRAVEL_DELETE_COPY(Buffer);

  Buffer(VmaAllocator allocator,
         uint64 size,
         VkSharingMode sharing_mode,
         VkBufferUsageFlagBits buffer_usage,
         VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_AUTO);

  ~Buffer();

  Buffer(Buffer&& other) noexcept;

  auto operator=(Buffer&& other) noexcept -> Buffer&;

  void set_data(const void* data, usize data_size);

  [[nodiscard]] auto get() -> VkBuffer { return mBuffer; }

 private:
  VmaAllocator mAllocator {VK_NULL_HANDLE};
  VkBuffer mBuffer {VK_NULL_HANDLE};
  VmaAllocation mAllocation {VK_NULL_HANDLE};

  void dispose() noexcept;

  [[nodiscard]] auto get_allocation_info() -> VmaAllocationInfo;
};

}  // namespace gravel::vlk
