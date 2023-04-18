#include "queue.hpp"

#include "common/primitives.hpp"
#include "graphics/vulkan/util/vk_call.hpp"

namespace glow::vk {

void submit_to_queue(VkQueue queue,
                     VkCommandBuffer cmd_buffer,
                     VkSemaphore wait_semaphore,
                     const VkPipelineStageFlags wait_dst_stage_mask,
                     VkSemaphore signal_semaphore,
                     VkFence fence)
{
  const uint32 wait_semaphore_count = (wait_semaphore != VK_NULL_HANDLE) ? 1 : 0;
  const uint32 signal_semaphore_count = (signal_semaphore != VK_NULL_HANDLE) ? 1 : 0;

  const auto* wait_dst_stage_mask_ptr =
      (wait_dst_stage_mask != 0) ? &wait_dst_stage_mask : nullptr;

  const VkSubmitInfo submit_info {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = nullptr,

      .waitSemaphoreCount = wait_semaphore_count,
      .pWaitSemaphores = &wait_semaphore,
      .pWaitDstStageMask = wait_dst_stage_mask_ptr,

      .commandBufferCount = 1,
      .pCommandBuffers = &cmd_buffer,

      .signalSemaphoreCount = signal_semaphore_count,
      .pSignalSemaphores = &signal_semaphore,
  };

  GLOW_VK_CALL(vkQueueSubmit(queue, 1, &submit_info, fence),
               "[VK] Could not submit command buffer to queue");
}

void wait_on_queue(VkQueue queue)
{
  GLOW_VK_CALL(vkQueueWaitIdle(queue), "[VK] Could not wait on queue");
}

}  // namespace glow::vk
