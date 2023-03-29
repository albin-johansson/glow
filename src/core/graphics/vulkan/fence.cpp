#include "fence.hpp"

#include <limits>  // numeric_limits

#include "common/debug/assert.hpp"
#include "common/primitives.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util.hpp"

namespace gravel::vlk {
namespace {

inline constexpr uint64 kMaxU64 = std::numeric_limits<uint64>::max();

}  // namespace

Fence::Fence(const bool signaled)
{
  GRAVEL_ASSERT(get_device() != VK_NULL_HANDLE);

  const VkFenceCreateFlags flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

  const VkFenceCreateInfo create_info {
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = flags,
  };

  GRAVEL_VK_CALL(vkCreateFence(get_device(), &create_info, nullptr, &mFence),
                 "[VK] Could not create fence");
}

Fence::~Fence() noexcept
{
  dispose();
}

void Fence::dispose() noexcept
{
  if (mFence != VK_NULL_HANDLE) {
    vkDestroyFence(get_device(), mFence, nullptr);
  }
}

Fence::Fence(Fence&& other) noexcept
    : mFence {other.mFence}
{
  other.mFence = VK_NULL_HANDLE;
}

auto Fence::operator=(Fence&& other) noexcept -> Fence&
{
  if (this != &other) {
    dispose();

    mFence = other.mFence;
    other.mFence = VK_NULL_HANDLE;
  }

  return *this;
}

void Fence::wait()
{
  GRAVEL_VK_CALL(vkWaitForFences(get_device(), 1, &mFence, VK_TRUE, kMaxU64),
                 "[VK] Could not wait for fence");
}

void Fence::reset()
{
  GRAVEL_VK_CALL(vkResetFences(get_device(), 1, &mFence), "[VK] Could not reset fence");
}

}  // namespace gravel::vlk
