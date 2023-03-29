#include "semaphore.hpp"

#include "common/debug/assert.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util.hpp"

namespace gravel::vlk {

Semaphore::Semaphore()
{
  GRAVEL_ASSERT(get_device() != VK_NULL_HANDLE);

  const VkSemaphoreCreateInfo create_info {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
  };

  GRAVEL_VK_CALL(vkCreateSemaphore(get_device(), &create_info, nullptr, &mSemaphore),
                 "[VK] Could not create semaphore");
}

Semaphore::~Semaphore() noexcept
{
  dispose();
}

void Semaphore::dispose() noexcept
{
  if (mSemaphore != VK_NULL_HANDLE) {
    vkDestroySemaphore(get_device(), mSemaphore, nullptr);
  }
}

Semaphore::Semaphore(Semaphore&& other) noexcept
    : mSemaphore {other.mSemaphore}
{
  other.mSemaphore = VK_NULL_HANDLE;
}

auto Semaphore::operator=(Semaphore&& other) noexcept -> Semaphore&
{
  if (this != &other) {
    dispose();

    mSemaphore = other.mSemaphore;
    other.mSemaphore = VK_NULL_HANDLE;
  }

  return *this;
}

}  // namespace gravel::vlk
