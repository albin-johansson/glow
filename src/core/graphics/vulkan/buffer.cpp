#include "buffer.hpp"

#include <algorithm>  // min
#include <cstring>    // memcpy

#include "common/debug/assert.hpp"
#include "graphics/vulkan/command_buffer.hpp"
#include "graphics/vulkan/util.hpp"

namespace gravel::vlk {

Buffer::Buffer(VmaAllocator allocator,
               const uint64 size,
               const VkBufferUsageFlags buffer_usage,
               const VkSharingMode sharing_mode,
               const VkMemoryPropertyFlags memory_properties,
               const VmaAllocationCreateFlags allocation_flags,
               const VmaMemoryUsage memory_usage)
    : mAllocator {allocator}
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

  GRAVEL_VK_CALL(vmaCreateBuffer(mAllocator,
                                 &buffer_info,
                                 &allocation_info,
                                 &mBuffer,
                                 &mAllocation,
                                 nullptr),
                 "[VK] Could not create buffer");
}

auto Buffer::staging(VmaAllocator allocator,
                     const uint64 size,
                     const VkBufferUsageFlags buffer_usage,
                     const VkSharingMode sharing_mode) -> Buffer
{
  const auto memory_properties =
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
  return Buffer {allocator,
                 size,
                 buffer_usage | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 sharing_mode,
                 memory_properties,
                 VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
                 VMA_MEMORY_USAGE_AUTO};
}

auto Buffer::gpu(VmaAllocator allocator,
                 const uint64 size,
                 const VkBufferUsageFlags buffer_usage,
                 const VkSharingMode sharing_mode) -> Buffer
{
  return Buffer {allocator,
                 size,
                 buffer_usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                 sharing_mode,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 0,
                 VMA_MEMORY_USAGE_CPU_TO_GPU};
}

auto Buffer::uniform(VmaAllocator allocator,
                     const uint64 size,
                     const VkSharingMode sharing_mode) -> Buffer
{
  return Buffer {
      allocator,
      size,
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      sharing_mode,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
      VMA_MEMORY_USAGE_AUTO};
}

Buffer::~Buffer()
{
  dispose();
}

void Buffer::dispose() noexcept
{
  if (mBuffer != VK_NULL_HANDLE) {
    GRAVEL_ASSERT(mAllocator != VK_NULL_HANDLE);
    GRAVEL_ASSERT(mAllocation != VK_NULL_HANDLE);

    vmaDestroyBuffer(mAllocator, mBuffer, mAllocation);
  }
}

Buffer::Buffer(Buffer&& other) noexcept
    : mAllocator {other.mAllocator},
      mBuffer {other.mBuffer},
      mAllocation {other.mAllocation}
{
  other.mAllocator = VK_NULL_HANDLE;
  other.mBuffer = VK_NULL_HANDLE;
  other.mAllocation = VK_NULL_HANDLE;
}

auto Buffer::operator=(Buffer&& other) noexcept -> Buffer&
{
  if (this != &other) {
    dispose();

    mAllocator = other.mAllocator;
    mBuffer = other.mBuffer;
    mAllocation = other.mAllocation;

    other.mAllocator = VK_NULL_HANDLE;
    other.mBuffer = VK_NULL_HANDLE;
    other.mAllocation = VK_NULL_HANDLE;
  }

  return *this;
}

void Buffer::set_data(const void* data, const usize data_size)
{
  // Get a pointer to mapped memory so that we can update the buffer from the CPU
  void* mapped_data = nullptr;
  GRAVEL_VK_CALL(vmaMapMemory(mAllocator, mAllocation, &mapped_data),
                 "[VK] Could not map memory");

  // Transfer the data, making sure not to write too much data into the buffer
  const auto allocation_info = get_allocation_info();
  const auto allocation_size = static_cast<usize>(allocation_info.size);
  std::memcpy(mapped_data, data, std::min(data_size, allocation_size));

  vmaUnmapMemory(mAllocator, mAllocation);
}

auto Buffer::get_allocation_info() -> VmaAllocationInfo
{
  VmaAllocationInfo info {};
  vmaGetAllocationInfo(mAllocator, mAllocation, &info);
  return info;
}

void copy_buffer(VulkanContext& context,
                 VkBuffer src_buffer,
                 VkBuffer dst_buffer,
                 const usize data_size)
{
  const auto command_buffers =
      create_command_buffers(context.device, context.command_pool, 1);
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

  GRAVEL_VK_CALL(vkQueueSubmit(context.graphics_queue, 1, &submit_info, VK_NULL_HANDLE),
                 "[VK] Could not submit command buffer for buffer copy");
  GRAVEL_VK_CALL(vkQueueWaitIdle(context.graphics_queue),
                 "[VK] Could not wait for queue after buffer copy");

  vkFreeCommandBuffers(context.device, context.command_pool, 1, &command_buffer);
}

auto create_buffer(VulkanContext& context,
                   VkBufferUsageFlags usage,
                   const void* data,
                   const usize data_size) -> Buffer
{
  auto staging_buffer = Buffer::staging(context.allocator, data_size, usage);
  staging_buffer.set_data(data, data_size);

  auto buffer = Buffer::gpu(context.allocator, data_size, usage);
  copy_buffer(context, staging_buffer.get(), buffer.get(), data_size);

  return buffer;
}

}  // namespace gravel::vlk
