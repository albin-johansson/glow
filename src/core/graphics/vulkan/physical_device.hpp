#pragma once

#include <vulkan/vulkan.h>

#include "common/primitives.hpp"
#include "common/type/maybe.hpp"
#include "common/type/vector.hpp"

namespace gravel::vlk {

struct QueueFamilyIndices final {
  Maybe<uint32> graphics_family;
  Maybe<uint32> present_family;
};

struct SwapChainSupport final {
  VkSurfaceCapabilitiesKHR capabilities {};
  Vector<VkSurfaceFormatKHR> formats;
  Vector<VkPresentModeKHR> present_modes;
};

/// Returns all available extensions for a specific GPU.
[[nodiscard]] auto get_extensions(VkPhysicalDevice gpu) -> Vector<VkExtensionProperties>;

/// Returns information about the queue families available for a specific GPU.
[[nodiscard]] auto get_queue_families(VkPhysicalDevice gpu)
    -> Vector<VkQueueFamilyProperties>;

/// Returns the supported surface formats for a given GPU and surface.
[[nodiscard]] auto get_surface_formats(VkPhysicalDevice gpu, VkSurfaceKHR surface)
    -> Vector<VkSurfaceFormatKHR>;

/// Returns the supported presentation modes for a given GPU and surface.
[[nodiscard]] auto get_present_modes(VkPhysicalDevice gpu, VkSurfaceKHR surface)
    -> Vector<VkPresentModeKHR>;

/// Returns indices of relevant queue families for a given GPU and surface.
[[nodiscard]] auto get_queue_family_indices(VkPhysicalDevice gpu, VkSurfaceKHR surface)
    -> QueueFamilyIndices;

/// Returns information about the swapchain attributes for a given GPU and surface.
[[nodiscard]] auto get_swapchain_support(VkPhysicalDevice gpu, VkSurfaceKHR surface)
    -> SwapChainSupport;

/// Returns a GPU that has been deemed suitable for use.
[[nodiscard]] auto get_suitable_physical_device(VkInstance instance, VkSurfaceKHR surface)
    -> VkPhysicalDevice;

}  // namespace gravel::vlk
