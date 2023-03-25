#include "allocator.hpp"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>  // Need to include the header again here, so that the definitions are generated

#include "common/debug/error.hpp"

namespace gravel::vlk {

Allocator::Allocator(VkInstance instance, VkPhysicalDevice gpu, VkDevice device)
{
  VmaAllocatorCreateInfo allocator_create_info {};
  allocator_create_info.physicalDevice = gpu;
  allocator_create_info.device = device;
  allocator_create_info.instance = instance;

  if (vmaCreateAllocator(&allocator_create_info, &mAllocator) != VK_SUCCESS) {
    throw Error {"[VK] Could not create allocator"};
  }
}

Allocator::~Allocator()
{
  vmaDestroyAllocator(mAllocator);
}

}  // namespace gravel::vlk
