#include "device.hpp"

#include "common/debug/error.hpp"
#include "common/type/set.hpp"
#include "common/type/vector.hpp"
#include "graphics/vulkan/physical_device.hpp"
#include "graphics/vulkan/util.hpp"

namespace gravel::vlk {

Device::Device(VkPhysicalDevice gpu, VkSurfaceKHR surface)
{
  const auto queue_family_indices = get_queue_family_indices(gpu, surface);

  const auto graphics_family_index = queue_family_indices.graphics_family.value();
  const auto present_family_index = queue_family_indices.present_family.value();

  const HashSet<uint32> unique_queue_families = {
      graphics_family_index,
      present_family_index,
  };

  Vector<VkDeviceQueueCreateInfo> queue_create_infos;
  queue_create_infos.reserve(unique_queue_families.size());

  const float priority = 1.0f;
  for (const auto queue_family_index : unique_queue_families) {
    queue_create_infos.push_back(VkDeviceQueueCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,

        .queueFamilyIndex = queue_family_index,
        .queueCount = 1,
        .pQueuePriorities = &priority,
    });
  }

  const VkPhysicalDeviceFeatures device_features {};

  VkDeviceCreateInfo device_create_info {
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,

      .queueCreateInfoCount = static_cast<uint32>(queue_create_infos.size()),
      .pQueueCreateInfos = queue_create_infos.data(),

      .enabledLayerCount = 0,
      .ppEnabledLayerNames = nullptr,

      .enabledExtensionCount = kRequiredDeviceExtensions.size(),
      .ppEnabledExtensionNames = kRequiredDeviceExtensions.data(),

      .pEnabledFeatures = &device_features,
  };

  if constexpr (GRAVEL_DEBUG_BUILD) {
    device_create_info.enabledLayerCount = kValidationLayerNames.size();
    device_create_info.ppEnabledLayerNames = kValidationLayerNames.data();
  }

  GRAVEL_VK_CALL(vkCreateDevice(gpu, &device_create_info, nullptr, &mDevice),
                 "[VK] Could not create logical device");

  vkGetDeviceQueue(mDevice, graphics_family_index, 0, &mGraphicsQueue);
  vkGetDeviceQueue(mDevice, present_family_index, 0, &mPresentQueue);
}

Device::~Device()
{
  vkDestroyDevice(mDevice, nullptr);
}

void Device::submit_rendering_commands(VkCommandBuffer command_buffer,
                                       VkSemaphore image_available_semaphore,
                                       VkSemaphore render_finished_semaphore,
                                       VkFence in_flight_fence)
{
  const VkPipelineStageFlags wait_dst_stage_mask =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  const VkSubmitInfo submit_info {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = nullptr,

      // Wait on the image_available_semaphore before command buffer execution
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &image_available_semaphore,
      .pWaitDstStageMask = &wait_dst_stage_mask,

      .commandBufferCount = 1,
      .pCommandBuffers = &command_buffer,

      // Signal the render_finished_semaphore after command buffer execution
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = &render_finished_semaphore,
  };

  GRAVEL_VK_CALL(vkQueueSubmit(mGraphicsQueue, 1, &submit_info, in_flight_fence),
                 "[VK] Could not submit command buffer to graphics queue");
}

void Device::present_swapchain_image(VkSwapchainKHR swapchain,
                                     const uint32 swapchain_image_index,
                                     VkSemaphore render_finished_semaphore)
{
  const VkPresentInfoKHR present_info {
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .pNext = nullptr,

      // Wait on the render_finished_semaphore before presentation
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &render_finished_semaphore,

      // Target swapchain image
      .swapchainCount = 1,
      .pSwapchains = &swapchain,
      .pImageIndices = &swapchain_image_index,

      .pResults = nullptr,
  };

  GRAVEL_VK_CALL(vkQueuePresentKHR(mPresentQueue, &present_info),
                 "[VK] Could not present swapchain image");
}

}  // namespace gravel::vlk