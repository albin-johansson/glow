#include "semaphore.hpp"

#include "common/debug/assert.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util/vk_call.hpp"

namespace glow::vk {

void SemaphoreDeleter::operator()(VkSemaphore semaphore) noexcept
{
  vkDestroySemaphore(get_device(), semaphore, nullptr);
}

auto create_semaphore() -> SemaphorePtr
{
  GLOW_ASSERT(get_device() != VK_NULL_HANDLE);

  const VkSemaphoreCreateInfo create_info {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
  };

  VkSemaphore semaphore = VK_NULL_HANDLE;
  GLOW_VK_CALL(vkCreateSemaphore(get_device(), &create_info, nullptr, &semaphore),
               "[VK] Could not create semaphore");

  return SemaphorePtr {semaphore};
}

}  // namespace glow::vk
