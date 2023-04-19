#include "command_pool.hpp"

#include "common/debug/assert.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util/vk_call.hpp"

namespace glow::vk {
namespace {

void allocate_command_buffers_helper(VkCommandPool cmd_pool,
                                     const uint32 count,
                                     VkCommandBuffer* out_buffers)
{
  GLOW_ASSERT(out_buffers != nullptr);

  const VkCommandBufferAllocateInfo alloc_info {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = nullptr,
      .commandPool = cmd_pool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = count,
  };

  GLOW_VK_CALL(vkAllocateCommandBuffers(get_device(), &alloc_info, out_buffers),
               "[VK] Could not allocate command buffers");
}

}  // namespace

void CommandPoolDeleter::operator()(VkCommandPool cmd_pool) noexcept
{
  vkDestroyCommandPool(get_device(), cmd_pool, nullptr);
}

auto create_command_pool(const uint32 queue_family_index,
                         const VkCommandPoolCreateFlags flags) -> CommandPoolPtr
{
  GLOW_ASSERT(get_device() != VK_NULL_HANDLE);

  const VkCommandPoolCreateInfo create_info {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .pNext = nullptr,
      .flags = flags,
      .queueFamilyIndex = queue_family_index,
  };

  VkCommandPool pool = VK_NULL_HANDLE;
  GLOW_VK_CALL(vkCreateCommandPool(get_device(), &create_info, nullptr, &pool),
               "[VK] Could not create command pool");

  return CommandPoolPtr {pool};
}

auto allocate_command_buffers(VkCommandPool cmd_pool, const uint32 count)
    -> Vector<VkCommandBuffer>
{
  Vector<VkCommandBuffer> cmd_buffers;
  cmd_buffers.resize(count);

  allocate_command_buffers_helper(cmd_pool, count, cmd_buffers.data());

  return cmd_buffers;
}

auto allocate_command_buffer(VkCommandPool cmd_pool) -> VkCommandBuffer
{
  VkCommandBuffer cmd_buffer = VK_NULL_HANDLE;
  allocate_command_buffers_helper(cmd_pool, 1, &cmd_buffer);

  return cmd_buffer;
}

}  // namespace glow::vk
