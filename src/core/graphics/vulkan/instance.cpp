#include "instance.hpp"

#include <SDL2/SDL_vulkan.h>
#include <spdlog/spdlog.h>

#include "common/debug/error.hpp"
#include "common/primitives.hpp"
#include "graphics/vulkan/util.hpp"

namespace gravel::vlk {
namespace {

[[nodiscard]] auto get_instance_extension_names(SDL_Window* window) -> Vector<const char*>
{
  uint32 extension_count = 0;
  SDL_Vulkan_GetInstanceExtensions(window, &extension_count, nullptr);

  Vector<const char*> extensions;
  extensions.resize(extension_count);
  SDL_Vulkan_GetInstanceExtensions(window, &extension_count, extensions.data());

#if GRAVEL_OS_MACOS
  extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif  // GRAVEL_OS_MACOS

  return extensions;
}

}  // namespace

Instance::Instance(SDL_Window* window)
{
  spdlog::debug("[VK] Creating Vulkan instance...");

  const auto extension_names = get_instance_extension_names(window);

  const VkApplicationInfo application_info {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = "Gravel",
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

      .enabledExtensionCount = static_cast<uint32>(extension_names.size()),
      .ppEnabledExtensionNames = extension_names.data(),
  };

#if GRAVEL_OS_MACOS
  // Allow implementations that only provide a subset of the Vulkan spec, e.g. MoltenVK.
  instance_create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif  // GRAVEL_OS_MACOS

  if constexpr (GRAVEL_DEBUG_BUILD) {
    spdlog::debug("[VK] Enabling validation layers");

    // Enable validation layers in debug builds
    instance_create_info.ppEnabledLayerNames = kValidationLayerNames.data();
    instance_create_info.enabledLayerCount = kValidationLayerNames.size();
  }

  if (vkCreateInstance(&instance_create_info, nullptr, &mInstance) != VK_SUCCESS) {
    throw Error {"[VK] Could not create Vulkan instance"};
  }
}

Instance::~Instance()
{
  vkDestroyInstance(mInstance, nullptr);
}

auto get_physical_devices(VkInstance instance) -> Vector<VkPhysicalDevice>
{
  uint32 gpu_count = 0;
  vkEnumeratePhysicalDevices(instance, &gpu_count, nullptr);

  Vector<VkPhysicalDevice> gpus;
  gpus.resize(gpu_count);

  vkEnumeratePhysicalDevices(instance, &gpu_count, gpus.data());

  return gpus;
}

}  // namespace gravel::vlk