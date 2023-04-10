#include "debug_messenger.hpp"

#include <exception>  // terminate

#include <spdlog/spdlog.h>

#include "common/debug/assert.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util.hpp"

namespace gravel::vk {
namespace {

VKAPI_ATTR auto debug_message_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                       VkDebugUtilsMessageTypeFlagsEXT,
                                       const VkDebugUtilsMessengerCallbackDataEXT* data,
                                       void*) -> VkBool32
{
  auto level = spdlog::level::debug;

  if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    level = spdlog::level::warn;
  }
  else if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    level = spdlog::level::err;
  }

  spdlog::log(level, "[Vulkan] {}", data->pMessage);

  if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    std::terminate();
  }

  return VK_FALSE;
}

}  // namespace

void DebugMessengerDeleter::operator()(VkDebugUtilsMessengerEXT messenger) noexcept
{
  destroy_debug_messenger(get_instance(), messenger);
}

auto create_debug_messenger() -> DebugMessenger
{
  GRAVEL_ASSERT(get_instance() != VK_NULL_HANDLE);

  const VkDebugUtilsMessengerCreateInfoEXT create_info {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .pNext = nullptr,
      .flags = 0,

      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,

      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,

      .pfnUserCallback = &debug_message_callback,
      .pUserData = nullptr,
  };

  VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
  GRAVEL_VK_CALL(create_debug_messenger(get_instance(), &create_info, &messenger),
                 "[VK] Could not create debug messenger");

  return DebugMessenger {messenger};
}

}  // namespace gravel::vk
