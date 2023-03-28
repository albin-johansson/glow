#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "graphics/vulkan/context.hpp"

namespace gravel::vlk {

class Buffer final {
 public:
  GRAVEL_DELETE_COPY(Buffer);

  Buffer(VmaAllocator allocator,
         uint64 size,
         VkBufferUsageFlags buffer_usage,
         VkSharingMode sharing_mode,
         VkMemoryPropertyFlags memory_properties,
         VmaAllocationCreateFlags allocation_flags,
         VmaMemoryUsage memory_usage);

  /// Creates a staging buffer.
  [[nodiscard]] static auto staging(
      VmaAllocator allocator,
      uint64 size,
      VkBufferUsageFlags buffer_usage,
      VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE) -> Buffer;

  /// Creates a GPU buffer that has to be updated with a staging buffer.
  [[nodiscard]] static auto gpu(VmaAllocator allocator,
                                uint64 size,
                                VkBufferUsageFlags buffer_usage,
                                VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE)
      -> Buffer;

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

/// Copies a buffer into another using the buffer copy draw command.
void copy_buffer(VulkanContext& context,
                 VkBuffer src_buffer,
                 VkBuffer dst_buffer,
                 usize data_size);

/// Creates an efficient buffer with the provided data (using a temporary staging buffer).
[[nodiscard]] auto create_buffer(VulkanContext& context,
                                 VkBufferUsageFlags usage,
                                 const void* data,
                                 usize data_size) -> Buffer;

}  // namespace gravel::vlk
