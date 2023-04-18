#include "instance.hpp"

#include <SDL2/SDL_vulkan.h>
#include <spdlog/spdlog.h>

#include "common/debug/error.hpp"
#include "common/primitives.hpp"
#include "common/type/vector.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util/constants.hpp"
#include "graphics/vulkan/util/size.hpp"
#include "graphics/vulkan/util/vk_call.hpp"
#include "init/window.hpp"
#include "util/arrays.hpp"

namespace glow::vk {
namespace {

[[nodiscard]] auto get_instance_extension_names(SDL_Window* window) -> Vector<const char*>
{
  uint32 extension_count = 0;
  SDL_Vulkan_GetInstanceExtensions(window, &extension_count, nullptr);

  Vector<const char*> extensions;
  extensions.resize(extension_count);
  SDL_Vulkan_GetInstanceExtensions(window, &extension_count, extensions.data());

#ifdef GLOW_USE_VULKAN_SUBSET
  extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif  // GLOW_USE_VULKAN_SUBSET

  if constexpr (kDebugBuild) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}

template <typename T>
void load_function(VkInstance instance, T& func, const char* name)
{
  func = reinterpret_cast<T>(vkGetInstanceProcAddr(instance, name));
}

}  // namespace

void InstanceDeleter::operator()(VkInstance instance) noexcept
{
  vkDestroyInstance(instance, nullptr);
}

auto create_instance() -> InstancePtr
{
  spdlog::debug("[VK] Creating Vulkan instance...");

  const auto extension_names = get_instance_extension_names(get_window());

  const VkApplicationInfo application_info {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,

      .pApplicationName = "Glow",
      .applicationVersion = VK_MAKE_VERSION(0, 1, 0),

      .pEngineName = "No Engine",
      .engineVersion = VK_MAKE_VERSION(0, 1, 0),

      .apiVersion = VK_API_VERSION_1_2,
  };

  VkInstanceCreateInfo instance_create_info {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,

      .flags = 0,
      .pApplicationInfo = &application_info,

      .enabledLayerCount = 0,
      .ppEnabledLayerNames = nullptr,

      .enabledExtensionCount = u32_size(extension_names),
      .ppEnabledExtensionNames = extension_names.data(),
  };

#ifdef GLOW_USE_VULKAN_SUBSET
  // Allow implementations that only provide a subset of the Vulkan spec, e.g. MoltenVK.
  instance_create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif  // GLOW_USE_VULKAN_SUBSET

  if constexpr (kDebugBuild) {
    spdlog::debug("[VK] Enabling validation layers");

    // Enable validation layers in debug builds
    instance_create_info.ppEnabledLayerNames = kValidationLayerNames;
    instance_create_info.enabledLayerCount = array_length(kValidationLayerNames);
  }

  VkInstance instance = VK_NULL_HANDLE;
  GLOW_VK_CALL(vkCreateInstance(&instance_create_info, nullptr, &instance),
               "[VK] Could not create instance");
  set_instance(instance);

  auto& functions = get_extension_functions();

  if constexpr (kDebugBuild) {
    load_function(instance,
                  functions.vkCreateDebugUtilsMessengerEXT,
                  "vkCreateDebugUtilsMessengerEXT");
    load_function(instance,
                  functions.vkDestroyDebugUtilsMessengerEXT,
                  "vkDestroyDebugUtilsMessengerEXT");
  }

  load_function(instance,
                functions.vkCmdPushDescriptorSetKHR,
                "vkCmdPushDescriptorSetKHR");
  load_function(instance,
                functions.vkCmdPushDescriptorSetWithTemplateKHR,
                "vkCmdPushDescriptorSetWithTemplateKHR");

  return InstancePtr {instance};
}

}  // namespace glow::vk