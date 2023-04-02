#include "physical_device.hpp"

#include <algorithm>  // max_element

#include "common/debug/error.hpp"
#include "common/primitives.hpp"
#include "common/type/array.hpp"
#include "common/type/map.hpp"
#include "common/type/set.hpp"
#include "common/type/string.hpp"
#include "common/type/vector.hpp"
#include "graphics/vulkan/instance.hpp"
#include "graphics/vulkan/util.hpp"

namespace gravel::vlk {
namespace {

[[nodiscard]] auto rate_gpu(VkPhysicalDevice gpu) -> int
{
  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(gpu, &properties);

  VkPhysicalDeviceFeatures features;
  vkGetPhysicalDeviceFeatures(gpu, &features);

  int score = 1;

  // Favor discrete (external) graphics cards
  if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    score += 100;
  }

  return score;
}

[[nodiscard]] auto has_required_extensions(VkPhysicalDevice gpu) -> bool
{
  HashSet<String> missing_extensions(kRequiredDeviceExtensions.begin(),
                                     kRequiredDeviceExtensions.end());

  const auto available_extensions = get_extensions(gpu);
  for (const auto& extension : available_extensions) {
    missing_extensions.erase(extension.extensionName);
  }

  return missing_extensions.empty();
}

[[nodiscard]] auto is_gpu_suitable(VkPhysicalDevice gpu, VkSurfaceKHR surface) -> bool
{
  const auto has_extensions = has_required_extensions(gpu);
  if (!has_extensions) {
    return false;
  }

  const auto queue_family_indices = get_queue_family_indices(gpu, surface);
  const auto has_queue_families = queue_family_indices.present_family.has_value() &&
                                  queue_family_indices.graphics_family.has_value();
  if (!has_queue_families) {
    return false;
  }

  VkPhysicalDeviceFeatures features {};
  vkGetPhysicalDeviceFeatures(gpu, &features);
  if (!features.samplerAnisotropy) {
    return false;
  }

  const auto swap_chain_support = get_swapchain_support(gpu, surface);
  return !swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();
}

}  // namespace

auto get_extensions(VkPhysicalDevice gpu) -> Vector<VkExtensionProperties>
{
  uint32 extension_count = 0;
  vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extension_count, nullptr);

  Vector<VkExtensionProperties> extensions;
  extensions.resize(extension_count);

  vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extension_count, extensions.data());

  return extensions;
}

auto get_queue_families(VkPhysicalDevice gpu) -> Vector<VkQueueFamilyProperties>
{
  uint32 queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_count, nullptr);

  Vector<VkQueueFamilyProperties> queue_families;
  queue_families.resize(queue_family_count);

  vkGetPhysicalDeviceQueueFamilyProperties(gpu,
                                           &queue_family_count,
                                           queue_families.data());

  return queue_families;
}

auto get_surface_formats(VkPhysicalDevice gpu, VkSurfaceKHR surface)
    -> Vector<VkSurfaceFormatKHR>
{
  uint32 format_count = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &format_count, nullptr);

  Vector<VkSurfaceFormatKHR> formats;
  formats.resize(format_count);

  vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &format_count, formats.data());

  return formats;
}

auto get_present_modes(VkPhysicalDevice gpu, VkSurfaceKHR surface)
    -> Vector<VkPresentModeKHR>
{
  uint32 present_mode_count = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &present_mode_count, nullptr);

  Vector<VkPresentModeKHR> present_modes;
  present_modes.resize(present_mode_count);

  vkGetPhysicalDeviceSurfacePresentModesKHR(gpu,
                                            surface,
                                            &present_mode_count,
                                            present_modes.data());

  return present_modes;
}

auto get_queue_family_indices(VkPhysicalDevice gpu, VkSurfaceKHR surface)
    -> QueueFamilyIndices
{
  QueueFamilyIndices indices;

  const auto queue_families = get_queue_families(gpu);

  uint32 index = 0;
  for (const auto& queue_family : queue_families) {
    if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics_family = index;
    }

    VkBool32 has_present_support = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(gpu, index, surface, &has_present_support);

    if (has_present_support) {
      indices.present_family = index;
    }

    ++index;
  }

  return indices;
}

auto get_swapchain_support(VkPhysicalDevice gpu, VkSurfaceKHR surface) -> SwapchainSupport
{
  SwapchainSupport support;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &support.capabilities);

  support.formats = get_surface_formats(gpu, surface);
  support.present_modes = get_present_modes(gpu, surface);

  return support;
}

auto get_suitable_physical_device(VkInstance instance, VkSurfaceKHR surface)
    -> VkPhysicalDevice
{
  const auto gpus = get_physical_devices(instance);

  if (gpus.empty()) {
    throw Error {"[VK] Found no GPUs with Vulkan support"};
  }

  // Rate available GPUs and select the best one
  HashMap<VkPhysicalDevice, int> gpu_scores;
  gpu_scores.reserve(gpus.size());

  for (const auto& gpu : gpus) {
    gpu_scores[gpu] = is_gpu_suitable(gpu, surface) ? rate_gpu(gpu) : -1;
  }

  const auto gpu_iter = std::max_element(gpu_scores.begin(), gpu_scores.end());
  auto* best_gpu = gpu_iter->first;

  if (best_gpu == VK_NULL_HANDLE) {
    throw Error {"[VK] Found no suitable GPU"};
  }

  return best_gpu;
}

}  // namespace gravel::vlk
