#include "buffer.hpp"

#include <algorithm>  // min
#include <cstring>    // memcpy

#include "common/debug/assert.hpp"
#include "graphics/vulkan/util.hpp"

namespace gravel::vlk {

Buffer::Buffer(VmaAllocator allocator,
               const uint64 size,
               const VkSharingMode sharing_mode,
               const VkBufferUsageFlagBits buffer_usage,
               const VmaMemoryUsage memory_usage)
    : mAllocator {allocator}
{
  const VkBufferCreateInfo buffer_create_info {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,

      .size = size,

      .usage = buffer_usage,
      .sharingMode = sharing_mode,

      .queueFamilyIndexCount = 0,
      .pQueueFamilyIndices = nullptr,
  };

  const VmaAllocationCreateInfo allocation_create_info {
      .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
      .usage = memory_usage,
      .requiredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      .memoryTypeBits = 0,
      .pool = nullptr,
      .pUserData = nullptr,
      .priority = 0,
  };

  GRAVEL_VK_CALL(vmaCreateBuffer(mAllocator,
                                 &buffer_create_info,
                                 &allocation_create_info,
                                 &mBuffer,
                                 &mAllocation,
                                 nullptr),
                 "[VK] Could not create buffer");
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

}  // namespace gravel::vlk
