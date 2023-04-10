#include "context.hpp"

namespace gravel::vk {
namespace {

inline ExtensionFunctions gFunctions;

inline VkInstance gInstance = VK_NULL_HANDLE;
inline VkPhysicalDevice gGPU = VK_NULL_HANDLE;
inline VkSurfaceKHR gSurface = VK_NULL_HANDLE;
inline VkDevice gDevice = VK_NULL_HANDLE;
inline VkQueue gGraphicsQueue = VK_NULL_HANDLE;
inline VkQueue gPresentationQueue = VK_NULL_HANDLE;
inline VkCommandPool gCommandPool = VK_NULL_HANDLE;
inline VmaAllocator gAllocator = VK_NULL_HANDLE;

}  // namespace

auto get_extension_functions() noexcept -> ExtensionFunctions&
{
  return gFunctions;
}

auto create_debug_messenger(VkInstance instance,
                            const VkDebugUtilsMessengerCreateInfoEXT* create_info,
                            VkDebugUtilsMessengerEXT* messenger) -> VkResult
{
  return gFunctions.vkCreateDebugUtilsMessengerEXT(instance,
                                                   create_info,
                                                   nullptr,
                                                   messenger);
}

void destroy_debug_messenger(VkInstance instance,
                             VkDebugUtilsMessengerEXT messenger) noexcept
{
  gFunctions.vkDestroyDebugUtilsMessengerEXT(instance, messenger, nullptr);
}

void push_descriptor_set(VkCommandBuffer cmd_buffer,
                         VkPipelineLayout layout,
                         const uint32 set,
                         const uint32 write_count,
                         const VkWriteDescriptorSet* writes)
{
  gFunctions.vkCmdPushDescriptorSetKHR(cmd_buffer,
                                       VK_PIPELINE_BIND_POINT_GRAPHICS,
                                       layout,
                                       set,
                                       write_count,
                                       writes);
}

void set_instance(VkInstance instance) noexcept
{
  gInstance = instance;
}

void set_surface(VkSurfaceKHR surface) noexcept
{
  gSurface = surface;
}

void set_gpu(VkPhysicalDevice gpu) noexcept
{
  gGPU = gpu;
}

void set_device(VkDevice device) noexcept
{
  gDevice = device;
}

void set_graphics_queue(VkQueue queue) noexcept
{
  gGraphicsQueue = queue;
}

void set_presentation_queue(VkQueue queue) noexcept
{
  gPresentationQueue = queue;
}

void set_command_pool(VkCommandPool pool) noexcept
{
  gCommandPool = pool;
}

void set_allocator(VmaAllocator allocator) noexcept
{
  gAllocator = allocator;
}

auto get_instance() noexcept -> VkInstance
{
  return gInstance;
}

auto get_surface() noexcept -> VkSurfaceKHR
{
  return gSurface;
}

auto get_gpu() noexcept -> VkPhysicalDevice
{
  return gGPU;
}

auto get_device() noexcept -> VkDevice
{
  return gDevice;
}

auto get_graphics_queue() noexcept -> VkQueue
{
  return gGraphicsQueue;
}

auto get_presentation_queue() noexcept -> VkQueue
{
  return gPresentationQueue;
}

auto get_command_pool() noexcept -> VkCommandPool
{
  return gCommandPool;
}

auto get_allocator() noexcept -> VmaAllocator
{
  return gAllocator;
}

}  // namespace gravel::vk
