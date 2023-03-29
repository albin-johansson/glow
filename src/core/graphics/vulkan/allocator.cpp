#include "allocator.hpp"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>  // Need to include the header again here, so that the definitions are generated

#include "common/debug/error.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util.hpp"

namespace gravel::vlk {

Allocator::Allocator(VkInstance instance, VkPhysicalDevice gpu, VkDevice device)
{
  VmaAllocatorCreateInfo allocator_create_info {};
  allocator_create_info.physicalDevice = gpu;
  allocator_create_info.device = device;
  allocator_create_info.instance = instance;

  GRAVEL_VK_CALL(vmaCreateAllocator(&allocator_create_info, &mAllocator),
                 "[VK] Could not create allocator");
  set_allocator(mAllocator);
}

Allocator::~Allocator()
{
  vmaDestroyAllocator(mAllocator);
}

}  // namespace gravel::vlk
