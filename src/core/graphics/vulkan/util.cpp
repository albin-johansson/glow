#include "util.hpp"

#include <magic_enum.hpp>

namespace gravel::vlk {

auto stringify_result(VkResult result) -> StringView
{
  return magic_enum::enum_name(result);
}

auto get_api_version(VkPhysicalDevice gpu) -> String
{
  VkPhysicalDeviceProperties gpu_properties {};
  vkGetPhysicalDeviceProperties(gpu, &gpu_properties);

  return fmt::format("{}.{}.{}",
                     VK_API_VERSION_MAJOR(gpu_properties.apiVersion),
                     VK_API_VERSION_MINOR(gpu_properties.apiVersion),
                     VK_API_VERSION_PATCH(gpu_properties.apiVersion));
}

auto get_driver_version(VkPhysicalDevice gpu) -> String
{
  VkPhysicalDeviceProperties gpu_properties {};
  vkGetPhysicalDeviceProperties(gpu, &gpu_properties);

  return fmt::format("{}.{}.{}",
                     VK_API_VERSION_MAJOR(gpu_properties.driverVersion),
                     VK_API_VERSION_MINOR(gpu_properties.driverVersion),
                     VK_API_VERSION_PATCH(gpu_properties.driverVersion));
}

}  // namespace gravel::vlk
