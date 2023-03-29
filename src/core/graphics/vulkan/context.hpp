#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace gravel::vlk {

/// Context component referencing fundamental Vulkan resources.
struct [[deprecated]] VulkanContext final {
  VkPhysicalDevice gpu {VK_NULL_HANDLE};
  VkDevice device {VK_NULL_HANDLE};
  VkQueue graphics_queue {VK_NULL_HANDLE};
  VkQueue presentation_queue {VK_NULL_HANDLE};
  VkCommandPool command_pool {VK_NULL_HANDLE};
  VmaAllocator allocator {VK_NULL_HANDLE};
};

void set_instance(VkInstance instance) noexcept;
void set_surface(VkSurfaceKHR surface) noexcept;
void set_gpu(VkPhysicalDevice gpu) noexcept;
void set_device(VkDevice device) noexcept;
void set_graphics_queue(VkQueue queue) noexcept;
void set_presentation_queue(VkQueue queue) noexcept;
void set_allocator(VmaAllocator allocator) noexcept;

[[nodiscard]] auto get_instance() noexcept -> VkInstance;
[[nodiscard]] auto get_surface() noexcept -> VkSurfaceKHR;
[[nodiscard]] auto get_gpu() noexcept -> VkPhysicalDevice;
[[nodiscard]] auto get_device() noexcept -> VkDevice;
[[nodiscard]] auto get_graphics_queue() noexcept -> VkQueue;
[[nodiscard]] auto get_presentation_queue() noexcept -> VkQueue;
[[nodiscard]] auto get_allocator() noexcept -> VmaAllocator;

}  // namespace gravel::vlk