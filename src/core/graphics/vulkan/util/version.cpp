#include "version.hpp"

#include <fmt/format.h>

namespace gravel::vk {

auto to_version_string(const uint32 version) -> String
{
  return fmt::format("{}.{}.{}",
                     VK_API_VERSION_MAJOR(version),
                     VK_API_VERSION_MINOR(version),
                     VK_API_VERSION_PATCH(version));
}

auto get_api_version(VkPhysicalDevice gpu) -> String
{
  VkPhysicalDeviceProperties properties {};
  vkGetPhysicalDeviceProperties(gpu, &properties);

  return to_version_string(properties.apiVersion);
}

auto get_driver_version(VkPhysicalDevice gpu) -> String
{
  VkPhysicalDeviceProperties properties {};
  vkGetPhysicalDeviceProperties(gpu, &properties);

  return to_version_string(properties.driverVersion);
}

}  // namespace gravel::vk
