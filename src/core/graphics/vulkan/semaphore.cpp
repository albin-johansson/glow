#include "semaphore.hpp"

#include "common/debug/assert.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util.hpp"

namespace gravel::vk {

void SemaphoreDeleter::operator()(VkSemaphore semaphore) noexcept
{
  vkDestroySemaphore(get_device(), semaphore, nullptr);
}

auto create_semaphore() -> Semaphore
{
  GRAVEL_ASSERT(get_device() != VK_NULL_HANDLE);

  const VkSemaphoreCreateInfo create_info {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
  };

  VkSemaphore semaphore = VK_NULL_HANDLE;
  GRAVEL_VK_CALL(vkCreateSemaphore(get_device(), &create_info, nullptr, &semaphore),
                 "[VK] Could not create semaphore");

  return Semaphore {semaphore};
}

}  // namespace gravel::vk
