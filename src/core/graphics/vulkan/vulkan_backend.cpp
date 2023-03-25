#include "vulkan_backend.hpp"

#include <SDL2/SDL_vulkan.h>
#include <fmt/format.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

#include "graphics/camera.hpp"
#include "graphics/renderer_info.hpp"
#include "graphics/vulkan/physical_device.hpp"
#include "scene/scene.hpp"
#include "scene/transform.hpp"

namespace gravel::vlk {

VulkanBackend::VulkanBackend(SDL_Window* window)
    : mInstance {window},
      mSurface {window, mInstance.get()},
      mGPU {get_suitable_physical_device(mInstance.get(), mSurface.get())},
      mDevice {mGPU, mSurface.get()},
      mSwapchain {window, mGPU, mDevice.get(), mSurface.get()},
      mRenderPass {mDevice.get(), mSwapchain.get_image_format()},
      mShadingPipeline {mDevice.get(), mRenderPass.get(), mSwapchain.get_image_extent()},
      mAllocator {mInstance.get(), mGPU, mDevice.get()}
{
  // TODO create framebuffers
  // TODO create command pool
  // TODO create command buffer
  // TODO create synchronization objects
}

void VulkanBackend::stop()
{
  mQuit = true;
}

void VulkanBackend::on_init(Scene& scene)
{
  prepare_imgui_for_vulkan();

  VkPhysicalDeviceProperties gpu_properties {};
  vkGetPhysicalDeviceProperties(mGPU, &gpu_properties);

  auto& renderer_info = scene.get<RendererInfo>();
  renderer_info.api = fmt::format("Vulkan {}.{}.{}",
                                  VK_API_VERSION_MAJOR(gpu_properties.apiVersion),
                                  VK_API_VERSION_MINOR(gpu_properties.apiVersion),
                                  VK_API_VERSION_PATCH(gpu_properties.apiVersion));
  renderer_info.renderer = gpu_properties.deviceName;
  renderer_info.vendor = "N/A";
  renderer_info.version = fmt::format("{}.{}.{}",
                                      VK_API_VERSION_MAJOR(gpu_properties.driverVersion),
                                      VK_API_VERSION_MINOR(gpu_properties.driverVersion),
                                      VK_API_VERSION_PATCH(gpu_properties.driverVersion));

  const auto camera_entity = scene.make_node("Main Camera");

  auto& camera = scene.add<Camera>(camera_entity);
  camera.up = Vec3 {0, 1, 0};

  auto& transform = scene.get<Transform>(camera_entity);
  transform.position = Vec3 {0, 0, 2};
  transform.rotation = Vec3 {0, 0, 1};

  auto& transform_options = scene.get<TransformOptions>(camera_entity);
  transform_options.use_rotation = false;
  transform_options.use_scale = false;

  auto& camera_context = scene.get<CameraContext>();
  camera_context.active_camera = camera_entity;
}

void VulkanBackend::prepare_imgui_for_vulkan()
{
  //  if (!ImGui_ImplVulkan_Init(&init_info, render_pass)) {
  //    throw Error {"[VK] Could not initialize DearImGui Vulkan backend"};
  //  }
}

void VulkanBackend::on_event(const SDL_Event& event)
{
  ImGui_ImplSDL2_ProcessEvent(&event);

  if (event.type == SDL_QUIT) {
    mQuit = true;
  }
}

void VulkanBackend::begin_frame() {}

void VulkanBackend::end_frame() {}

void VulkanBackend::render_scene(const Scene& scene,
                                 const Vec2& framebuffer_size,
                                 Dispatcher& dispatcher)
{
}

auto VulkanBackend::get_primary_framebuffer_handle() -> void*
{
  return nullptr;
}

}  // namespace gravel::vlk
