#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/memory.hpp"
#include "common/type/vector.hpp"

namespace glow::vk {

struct CommandPoolDeleter final {
  void operator()(VkCommandPool pool) noexcept;
};

using CommandPoolPtr = Unique<VkCommandPool_T, CommandPoolDeleter>;

/// Creates a command pool for a specific queue.
///
/// \param queue_family_index index of the queue family that derived command buffers will
/// be submitted to.
/// \param flags optional command pool creation flags.
[[nodiscard]] auto create_command_pool(uint32 queue_family_index,
                                       VkCommandPoolCreateFlags flags = 0)
    -> CommandPoolPtr;

/// Allocates a specific amount of command buffers from a pool.
[[nodiscard]] auto allocate_command_buffers(VkCommandPool command_pool, uint32 count)
    -> Vector<VkCommandBuffer>;

/// Allocates a single command buffer from a pool.
[[nodiscard]] auto allocate_command_buffer(VkCommandPool command_pool) -> VkCommandBuffer;

}  // namespace glow::vk
