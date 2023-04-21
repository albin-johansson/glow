#pragma once

#include <functional>  // function

#include <vulkan/vulkan.h>

#include "common/primitives.hpp"

namespace glow::vk {

using UnaryCmdBufferFunc = std::function<void(VkCommandBuffer)>;

/// Resets the state of a command buffer.
///
/// \note The associated command pool must have been created with the
/// `VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT` flag.
///
/// \param cmd_buffer the command buffer that will be reset.
void reset_command_buffer(VkCommandBuffer cmd_buffer);

/// Initializes a command buffer for command recording.
///
/// \param cmd_buffer the command buffer to begin.
/// \param flags optional bitmask of flags.
void begin_command_buffer(VkCommandBuffer cmd_buffer,
                          VkCommandBufferUsageFlags flags = 0);

/// Ends the recording of a command buffer.
void end_command_buffer(VkCommandBuffer cmd_buffer);

/// Records a one time command buffer, and executes it (and waits for it to complete).
///
/// \param cmd_pool the command pool to allocate the command buffer with.
/// \param func a function object invoked with the temporary command buffer.
void execute(VkCommandPool cmd_pool, const UnaryCmdBufferFunc& func);

}  // namespace glow::vk
