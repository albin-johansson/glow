#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"

namespace gravel::vlk {

class Device final {
 public:
  GRAVEL_DELETE_COPY(Device);
  GRAVEL_DELETE_MOVE(Device);

  Device(VkPhysicalDevice gpu, VkSurfaceKHR surface);

  ~Device();

  /// Submits rendering commands to the graphics queue.
  void submit_rendering_commands(VkCommandBuffer command_buffer,
                                 VkSemaphore image_available_semaphore,
                                 VkSemaphore render_finished_semaphore,
                                 VkFence in_flight_fence);

  /// Presents a swapchain image to the presentation queue.
  void present_swapchain_image(VkSwapchainKHR swapchain,
                               uint32 swapchain_image_index,
                               VkSemaphore render_finished_semaphore);

  [[nodiscard]] auto get() -> VkDevice { return mDevice; }
  [[nodiscard]] auto get_graphics_queue() -> VkQueue { return mGraphicsQueue; }
  [[nodiscard]] auto get_present_queue() -> VkQueue { return mPresentQueue; }

 private:
  VkDevice mDevice {VK_NULL_HANDLE};
  VkQueue mGraphicsQueue {VK_NULL_HANDLE};
  VkQueue mPresentQueue {VK_NULL_HANDLE};
};

}  // namespace gravel::vlk
