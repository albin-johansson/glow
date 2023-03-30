#include "descriptor_pool.hpp"

#include "common/debug/assert.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util.hpp"
#include "util/arrays.hpp"

namespace gravel::vlk {

DescriptorPool::DescriptorPool(const VkDescriptorPoolSize* pool_sizes,
                               const uint32 pool_size_count)
{
  GRAVEL_ASSERT(get_device() != VK_NULL_HANDLE);

  const VkDescriptorPoolCreateInfo pool_info {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,

      .pNext = nullptr,
      .flags = 0,

      .maxSets = kMaxFramesInFlight,

      .poolSizeCount = pool_size_count,
      .pPoolSizes = pool_sizes,
  };

  GRAVEL_VK_CALL(vkCreateDescriptorPool(get_device(), &pool_info, nullptr, &mPool),
                 "[VK] Could not create descriptor pool");
}

DescriptorPool::~DescriptorPool() noexcept
{
  dispose();
}

void DescriptorPool::dispose() noexcept
{
  if (mPool != VK_NULL_HANDLE) {
    vkDestroyDescriptorPool(get_device(), mPool, nullptr);
  }
}

DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept
    : mPool {other.mPool}
{
  other.mPool = VK_NULL_HANDLE;
}

auto DescriptorPool::operator=(DescriptorPool&& other) noexcept -> DescriptorPool&
{
  if (this != &other) {
    dispose();

    mPool = other.mPool;
    other.mPool = VK_NULL_HANDLE;
  }

  return *this;
}

}  // namespace gravel::vlk
