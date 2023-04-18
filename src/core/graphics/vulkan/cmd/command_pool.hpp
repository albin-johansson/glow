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

/// Creates a command pool for the graphics queue.
[[nodiscard]] auto create_command_pool(VkCommandPoolCreateFlags flags) -> CommandPoolPtr;

/// Allocates a specific amount of command buffers from a pool.
[[nodiscard]] auto allocate_command_buffers(VkCommandPool command_pool, uint32 count)
    -> Vector<VkCommandBuffer>;

/// Allocates a single command buffer from a pool.
[[nodiscard]] auto allocate_command_buffer(VkCommandPool command_pool) -> VkCommandBuffer;

}  // namespace glow::vk
