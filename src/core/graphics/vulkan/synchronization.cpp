#include "synchronization.hpp"

#include "common/debug/error.hpp"
#include "graphics/vulkan/util.hpp"

namespace gravel::vlk {

void wait_and_then_reset_fence(VkDevice device, VkFence fence)
{
  GRAVEL_VK_CALL(vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX),
                 "[VK] Could not wait for fence");
  GRAVEL_VK_CALL(vkResetFences(device, 1, &fence), "[VK] Could not reset fence");
}

auto create_semaphore(VkDevice device) -> VkSemaphore
{
  const VkSemaphoreCreateInfo semaphore_create_info {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
  };

  VkSemaphore semaphore {VK_NULL_HANDLE};
  GRAVEL_VK_CALL(vkCreateSemaphore(device, &semaphore_create_info, nullptr, &semaphore),
                 "[VK] Could not create semaphore");

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
  GRAVEL_VK_CALL(vkCreateFence(device, &fence_create_info, nullptr, &fence),
                 "[VK] Could not create fence");

  return fence;
}

}  // namespace gravel::vlk
