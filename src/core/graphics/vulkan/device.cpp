#include "device.hpp"

#include "common/debug/assert.hpp"
#include "common/debug/error.hpp"
#include "common/type/set.hpp"
#include "common/type/vector.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/physical_device.hpp"
#include "graphics/vulkan/util/constants.hpp"
#include "graphics/vulkan/util/size.hpp"
#include "graphics/vulkan/util/vk_call.hpp"
#include "util/arrays.hpp"

namespace gravel::vk {

void DeviceDeleter::operator()(VkDevice device) noexcept
{
  vkDestroyDevice(device, nullptr);
}

auto create_device() -> Device
{
  GRAVEL_ASSERT(get_gpu() != VK_NULL_HANDLE);
  GRAVEL_ASSERT(get_surface() != VK_NULL_HANDLE);

  const auto queue_family_indices = get_queue_family_indices(get_gpu(), get_surface());

  const auto graphics_family_index = queue_family_indices.graphics_family.value();
  const auto present_family_index = queue_family_indices.present_family.value();

  const HashSet<uint32> unique_queue_families = {
      graphics_family_index,
      present_family_index,
  };

  Vector<VkDeviceQueueCreateInfo> queue_create_infos;
  queue_create_infos.reserve(unique_queue_families.size());

  const float queue_priority = 1.0f;

  for (const auto queue_family_index : unique_queue_families) {
    queue_create_infos.push_back(VkDeviceQueueCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = queue_family_index,
        .queueCount = 1,
        .pQueuePriorities = &queue_priority,
    });
  }

  VkPhysicalDeviceFeatures enabled_device_features {};
  enabled_device_features.samplerAnisotropy = VK_TRUE;
  enabled_device_features.fillModeNonSolid = VK_TRUE;

  VkPhysicalDeviceDescriptorIndexingFeatures indexing_features {};
  indexing_features.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
  indexing_features.descriptorBindingPartiallyBound = VK_TRUE;

  VkDeviceCreateInfo device_create_info {
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pNext = &indexing_features,

      .queueCreateInfoCount = u32_size(queue_create_infos),
      .pQueueCreateInfos = queue_create_infos.data(),

      .enabledLayerCount = 0,
      .ppEnabledLayerNames = nullptr,

      .enabledExtensionCount = array_length(kRequiredDeviceExtensions),
      .ppEnabledExtensionNames = kRequiredDeviceExtensions,

      .pEnabledFeatures = &enabled_device_features,
  };

  if constexpr (GRAVEL_DEBUG_BUILD) {
    device_create_info.enabledLayerCount = array_length(kValidationLayerNames);
    device_create_info.ppEnabledLayerNames = kValidationLayerNames;
  }

  VkDevice device = VK_NULL_HANDLE;
  GRAVEL_VK_CALL(vkCreateDevice(get_gpu(), &device_create_info, nullptr, &device),
                 "[VK] Could not create Vulkan device");
  set_device(device);

  VkQueue graphics_queue = VK_NULL_HANDLE;
  VkQueue presentation_queue = VK_NULL_HANDLE;

  vkGetDeviceQueue(device, graphics_family_index, 0, &graphics_queue);
  vkGetDeviceQueue(device, present_family_index, 0, &presentation_queue);

  set_graphics_queue(graphics_queue);
  set_presentation_queue(presentation_queue);

  return Device {device};
}

}  // namespace gravel::vk
