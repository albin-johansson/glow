#pragma once

#include <vulkan/vulkan.h>

namespace glow::vk {

/// Submits a command buffer to a queue for execution.
///
/// \param queue the queue to submit the commands to.
/// \param cmd_buffer the command buffer with commands to submit.
/// \param wait_semaphore semaphore to wait on before submitting the command buffer.
/// \param wait_dst_stage_mask pipeline stage in which the waiting will occur.
/// \param signal_semaphore semaphore that will be signaled after the command buffer has
/// been executed.
/// \param fence a fence that will be signaled once the command buffer has been executed.
void submit_to_queue(VkQueue queue,
                     VkCommandBuffer cmd_buffer,
                     VkSemaphore wait_semaphore = VK_NULL_HANDLE,
                     VkPipelineStageFlags wait_dst_stage_mask = 0,
                     VkSemaphore signal_semaphore = VK_NULL_HANDLE,
                     VkFence fence = VK_NULL_HANDLE);

/// Waits (blocks the calling thread) until the work in a queue has been completed.
void wait_on_queue(VkQueue queue);

}  // namespace glow::vk
