#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "graphics/vulkan/context.hpp"

namespace glow::vk {

class Buffer final {
 public:
  GRAVEL_DELETE_COPY(Buffer);

  Buffer(uint64 size,
         VkBufferUsageFlags buffer_usage,
         VkMemoryPropertyFlags memory_properties,
         VmaAllocationCreateFlags allocation_flags,
         VmaMemoryUsage memory_usage);

  /// Creates a staging buffer.
  [[nodiscard]] static auto staging(uint64 size, VkBufferUsageFlags buffer_usage)
      -> Buffer;

  /// Creates a GPU buffer that has to be updated with a staging buffer.
  [[nodiscard]] static auto gpu(uint64 size, VkBufferUsageFlags buffer_usage) -> Buffer;

  /// Creates a uniform buffer that remains mapped for its entire lifetime.
  [[nodiscard]] static auto uniform(uint64 size) -> Buffer;

  /// Creates an efficient buffer with the provided data (using a staging buffer).
  [[nodiscard]] static auto create(VkBufferUsageFlags usage,
                                   const void* data,
                                   usize data_size) -> Buffer;

  ~Buffer();

  Buffer(Buffer&& other) noexcept;

  auto operator=(Buffer&& other) noexcept -> Buffer&;

  void set_data(const void* data, usize data_size);

  void bind_as_vertex_buffer(VkCommandBuffer command_buffer) const;

  void bind_as_index_buffer(VkCommandBuffer command_buffer, VkIndexType type) const;

  [[nodiscard]] auto get() -> VkBuffer { return mBuffer; }

 private:
  VkBuffer mBuffer {VK_NULL_HANDLE};
  VmaAllocation mAllocation {VK_NULL_HANDLE};

  void dispose() noexcept;

  [[nodiscard]] auto get_allocation_info() -> VmaAllocationInfo;
};

/// Copies a buffer into another using the buffer copy draw command.
void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, usize data_size);

}  // namespace glow::vk
