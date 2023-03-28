#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace gravel::vlk {

/// Context component referencing fundamental Vulkan resources.
struct VulkanContext final {
  VkPhysicalDevice gpu {VK_NULL_HANDLE};
  VkDevice device {VK_NULL_HANDLE};
  VkQueue graphics_queue {VK_NULL_HANDLE};
  VkQueue presentation_queue {VK_NULL_HANDLE};
  VkCommandPool command_pool {VK_NULL_HANDLE};
  VmaAllocator allocator {VK_NULL_HANDLE};
};

}  // namespace gravel::vlk