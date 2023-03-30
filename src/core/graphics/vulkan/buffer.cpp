#include "buffer.hpp"

#include <algorithm>  // min
#include <cstring>    // memcpy

#include "common/debug/assert.hpp"
#include "graphics/vulkan/cmd/command_buffer.hpp"
#include "graphics/vulkan/cmd/commands.hpp"
#include "graphics/vulkan/util.hpp"

namespace gravel::vlk {

Buffer::Buffer(const uint64 size,
               const VkBufferUsageFlags buffer_usage,
               const VkSharingMode sharing_mode,
               const VkMemoryPropertyFlags memory_properties,
               const VmaAllocationCreateFlags allocation_flags,
               const VmaMemoryUsage memory_usage)
{
  const VkBufferCreateInfo buffer_info {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,

      .size = size,

      .usage = buffer_usage,
      .sharingMode = sharing_mode,

      .queueFamilyIndexCount = 0,
      .pQueueFamilyIndices = nullptr,
  };

  const VmaAllocationCreateInfo allocation_info {
      .flags = allocation_flags,
      .usage = memory_usage,
      .requiredFlags = memory_properties,
      .memoryTypeBits = 0,
      .pool = nullptr,
      .pUserData = nullptr,
      .priority = 0,
  };

  GRAVEL_VK_CALL(vmaCreateBuffer(get_allocator(),
                                 &buffer_info,
                                 &allocation_info,
                                 &mBuffer,
                                 &mAllocation,
                                 nullptr),
                 "[VK] Could not create buffer");
}

auto Buffer::staging(const uint64 size,
                     const VkBufferUsageFlags buffer_usage,
                     const VkSharingMode sharing_mode) -> Buffer
{
  const auto memory_properties =
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
  return Buffer {size,
                 buffer_usage | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 sharing_mode,
                 memory_properties,
                 VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
                 VMA_MEMORY_USAGE_AUTO};
}

auto Buffer::gpu(const uint64 size,
                 const VkBufferUsageFlags buffer_usage,
                 const VkSharingMode sharing_mode) -> Buffer
{
  return Buffer {size,
                 buffer_usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                 sharing_mode,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 0,
                 VMA_MEMORY_USAGE_CPU_TO_GPU};
}

auto Buffer::uniform(const uint64 size, const VkSharingMode sharing_mode) -> Buffer
{
  return Buffer {
      size,
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      sharing_mode,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
      VMA_MEMORY_USAGE_AUTO};
}

auto Buffer::create(const VkBufferUsageFlags usage,
                    const void* data,
                    const usize data_size) -> Buffer
{
  auto staging_buffer = Buffer::staging(data_size, usage);
  staging_buffer.set_data(data, data_size);

  auto buffer = Buffer::gpu(data_size, usage);
  copy_buffer(staging_buffer.get(), buffer.get(), data_size);

  return buffer;
}

Buffer::~Buffer()
{
  dispose();
}

void Buffer::dispose() noexcept
{
  if (mBuffer != VK_NULL_HANDLE) {
    GRAVEL_ASSERT(mAllocation != VK_NULL_HANDLE);

    vmaDestroyBuffer(get_allocator(), mBuffer, mAllocation);
  }
}

Buffer::Buffer(Buffer&& other) noexcept
    : mBuffer {other.mBuffer},
      mAllocation {other.mAllocation}
{
  other.mBuffer = VK_NULL_HANDLE;
  other.mAllocation = VK_NULL_HANDLE;
}

auto Buffer::operator=(Buffer&& other) noexcept -> Buffer&
{
  if (this != &other) {
    dispose();

    mBuffer = other.mBuffer;
    mAllocation = other.mAllocation;

    other.mBuffer = VK_NULL_HANDLE;
    other.mAllocation = VK_NULL_HANDLE;
  }

  return *this;
}

void Buffer::set_data(const void* data, const usize data_size)
{
  // Get a pointer to mapped memory so that we can update the buffer from the CPU
  void* mapped_data = nullptr;
  GRAVEL_VK_CALL(vmaMapMemory(get_allocator(), mAllocation, &mapped_data),
                 "[VK] Could not map memory");

  // Transfer the data, making sure not to write too much data into the buffer
  const auto allocation_info = get_allocation_info();
  const auto allocation_size = static_cast<usize>(allocation_info.size);
  std::memcpy(mapped_data, data, std::min(data_size, allocation_size));

  vmaUnmapMemory(get_allocator(), mAllocation);
}

void Buffer::bind_as_vertex_buffer(VkCommandBuffer command_buffer) const
{
  const VkDeviceSize offset = 0;
  vkCmdBindVertexBuffers(command_buffer, 0, 1, &mBuffer, &offset);
}

void Buffer::bind_as_index_buffer(VkCommandBuffer command_buffer, VkIndexType type) const
{
  vkCmdBindIndexBuffer(command_buffer, mBuffer, 0, type);
}

auto Buffer::get_allocation_info() -> VmaAllocationInfo
{
  VmaAllocationInfo info {};
  vmaGetAllocationInfo(get_allocator(), mAllocation, &info);
  return info;
}

void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, const usize data_size)
{
  VkCommandPool command_pool = get_command_pool();

  const auto command_buffers = create_command_buffers(get_device(), command_pool, 1);
  VkCommandBuffer command_buffer = command_buffers.at(0);

  begin_command_buffer(command_buffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
  cmd::copy_buffer(command_buffer, src_buffer, dst_buffer, data_size);
  end_command_buffer(command_buffer);

  const VkSubmitInfo submit_info {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = nullptr,

      .waitSemaphoreCount = 0,
      .pWaitSemaphores = nullptr,
      .pWaitDstStageMask = nullptr,

      .commandBufferCount = 1,
      .pCommandBuffers = &command_buffer,

      .signalSemaphoreCount = 0,
      .pSignalSemaphores = nullptr,
  };

  VkQueue graphics_queue = get_graphics_queue();
  GRAVEL_VK_CALL(vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE),
                 "[VK] Could not submit command buffer for buffer copy");
  GRAVEL_VK_CALL(vkQueueWaitIdle(graphics_queue),
                 "[VK] Could not wait for queue after buffer copy");

  vkFreeCommandBuffers(get_device(), command_pool, 1, &command_buffer);
}

}  // namespace gravel::vlk
