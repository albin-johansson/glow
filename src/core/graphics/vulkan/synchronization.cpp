#include "synchronization.hpp"

#include "common/debug/error.hpp"

namespace gravel::vlk {

auto create_semaphore(VkDevice device) -> VkSemaphore
{
  const VkSemaphoreCreateInfo semaphore_create_info {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
  };

  VkSemaphore semaphore {VK_NULL_HANDLE};
  if (vkCreateSemaphore(device, &semaphore_create_info, nullptr, &semaphore) !=
      VK_SUCCESS) {
    throw Error {"[VK] Could not create semaphore"};
  }

  return semaphore;
}

auto create_signaled_fence(VkDevice device) -> VkFence
{
  const VkFenceCreateInfo fence_create_info {
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = VK_FENCE_CREATE_SIGNALED_BIT,
  };

  VkFence fence {VK_NULL_HANDLE};
  if (vkCreateFence(device, &fence_create_info, nullptr, &fence) != VK_SUCCESS) {
    throw Error {"[VK] Could not create fence"};
  }

  return fence;
}

}  // namespace gravel::vlk
