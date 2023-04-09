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

/// Stores extension function pointers.
struct ExtensionFunctions final {
  PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT {};
  PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT {};
  PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR {};
  PFN_vkCmdPushDescriptorSetWithTemplateKHR vkCmdPushDescriptorSetWithTemplateKHR {};
};

[[nodiscard]] auto get_extension_functions() noexcept -> ExtensionFunctions&;

void set_instance(VkInstance instance) noexcept;
void set_surface(VkSurfaceKHR surface) noexcept;
void set_gpu(VkPhysicalDevice gpu) noexcept;
void set_device(VkDevice device) noexcept;
void set_graphics_queue(VkQueue queue) noexcept;
void set_presentation_queue(VkQueue queue) noexcept;
void set_command_pool(VkCommandPool pool) noexcept;
void set_allocator(VmaAllocator allocator) noexcept;

[[nodiscard]] auto get_instance() noexcept -> VkInstance;
[[nodiscard]] auto get_surface() noexcept -> VkSurfaceKHR;
[[nodiscard]] auto get_gpu() noexcept -> VkPhysicalDevice;
[[nodiscard]] auto get_device() noexcept -> VkDevice;
[[nodiscard]] auto get_graphics_queue() noexcept -> VkQueue;
[[nodiscard]] auto get_presentation_queue() noexcept -> VkQueue;
[[nodiscard]] auto get_command_pool() noexcept -> VkCommandPool;
[[nodiscard]] auto get_allocator() noexcept -> VmaAllocator;

}  // namespace gravel::vlk