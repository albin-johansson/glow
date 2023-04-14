#include "allocator.hpp"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>  // Need to include the header again here, so that the definitions are generated

#include "common/debug/assert.hpp"
#include "common/debug/error.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util/vk_call.hpp"

namespace gravel::vk {

void AllocatorDeleter::operator()(VmaAllocator allocator) noexcept
{
  vmaDestroyAllocator(allocator);
}

auto create_allocator() -> Allocator
{
  GRAVEL_ASSERT(get_instance() != VK_NULL_HANDLE);
  GRAVEL_ASSERT(get_gpu() != VK_NULL_HANDLE);
  GRAVEL_ASSERT(get_device() != VK_NULL_HANDLE);

  VmaAllocatorCreateInfo create_info {};
  create_info.instance = get_instance();
  create_info.physicalDevice = get_gpu();
  create_info.device = get_device();

  VmaAllocator allocator = VK_NULL_HANDLE;
  GRAVEL_VK_CALL(vmaCreateAllocator(&create_info, &allocator),
                 "[VK] Could not create allocator");
  set_allocator(allocator);

  return Allocator {allocator};
}

}  // namespace gravel::vk
