#include "allocator.hpp"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>  // Need to include the header again here, so that the definitions are generated

#include "common/debug/assert.hpp"
#include "common/debug/error.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util.hpp"

namespace gravel::vk {

Allocator::Allocator()
{
  GRAVEL_ASSERT(get_instance() != VK_NULL_HANDLE);
  GRAVEL_ASSERT(get_gpu() != VK_NULL_HANDLE);
  GRAVEL_ASSERT(get_device() != VK_NULL_HANDLE);

  VmaAllocatorCreateInfo create_info {};
  create_info.instance = get_instance();
  create_info.physicalDevice = get_gpu();
  create_info.device = get_device();

  GRAVEL_VK_CALL(vmaCreateAllocator(&create_info, &mAllocator),
                 "[VK] Could not create allocator");
  set_allocator(mAllocator);
}

Allocator::~Allocator() noexcept
{
  vmaDestroyAllocator(mAllocator);
}

}  // namespace gravel::vk
