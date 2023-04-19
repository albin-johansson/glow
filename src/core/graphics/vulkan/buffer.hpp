#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "graphics/vulkan/context.hpp"

namespace glow::vk {

/// Represents a general Vulkan buffer.
class Buffer final {
 public:
  GLOW_DELETE_COPY(Buffer);

  /// Creates an empty buffer.
  ///
  /// \param size the total size of the buffer, in bytes.
  /// \param buffer_usage bitmask of buffer usage flags.
  /// \param memory_properties bitmask of required buffer memory properties.
  /// \param allocation_flags bitmask of buffer memory allocation flags.
  /// \param memory_usage allocator usage hint for the memory allocator.
  Buffer(uint64 size,
         VkBufferUsageFlags buffer_usage,
         VkMemoryPropertyFlags memory_properties,
         VmaAllocationCreateFlags allocation_flags,
         VmaMemoryUsage memory_usage);

  /// Creates a CPU-side staging buffer.
  [[nodiscard]] static auto staging(uint64 size, VkBufferUsageFlags buffer_usage)
      -> Buffer;

  /// Creates a GPU-side buffer that has to be updated with a staging buffer.
  [[nodiscard]] static auto gpu(uint64 size, VkBufferUsageFlags buffer_usage) -> Buffer;

  /// Creates a GPU-side uniform buffer that remains mapped for its entire lifetime.
  [[nodiscard]] static auto uniform(uint64 size) -> Buffer;

  /// Creates an GPU-side buffer with the provided data (using a staging buffer).
  [[nodiscard]] static auto create(VkBufferUsageFlags usage,
                                   const void* data,
                                   usize data_size) -> Buffer;

  ~Buffer() noexcept;

  Buffer(Buffer&& other) noexcept;

  auto operator=(Buffer&& other) noexcept -> Buffer&;

  /// Copies data into the buffer.
  ///
  /// \note This function is only usable for buffers that are host-visible, i.e.
  /// accessible by the CPU, such as uniform buffers.
  ///
  /// \param data the raw data to copy into the buffer.
  /// \param data_size the size of the data to write into the buffer, in bytes.
  void set_data(const void* data, usize data_size);

  /// Binds the contents of the buffer as vertex data.
  void bind_as_vertex_buffer(VkCommandBuffer cmd_buffer) const;

  /// Binds the contents of the buffer as vertex indices.
  ///
  /// \param cmd_buffer the target command buffer.
  /// \param type the type of the index values.
  void bind_as_index_buffer(VkCommandBuffer cmd_buffer, VkIndexType type) const;

  [[nodiscard]] auto get() -> VkBuffer { return mBuffer; }

 private:
  VkBuffer mBuffer {VK_NULL_HANDLE};
  VmaAllocation mAllocation {VK_NULL_HANDLE};

  void dispose() noexcept;

  [[nodiscard]] auto get_allocation_info() -> VmaAllocationInfo;
};

}  // namespace glow::vk
