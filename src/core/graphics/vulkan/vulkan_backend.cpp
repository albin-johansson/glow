#include "vulkan_backend.hpp"

#include <SDL2/SDL_vulkan.h>
#include <fmt/format.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

#include "graphics/camera.hpp"
#include "graphics/renderer_info.hpp"
#include "graphics/vulkan/command_buffer.hpp"
#include "graphics/vulkan/physical_device.hpp"
#include "graphics/vulkan/synchronization.hpp"
#include "graphics/vulkan/util.hpp"
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

      mCommandPool {create_command_pool(mDevice.get(), mGPU, mSurface.get())},
      mCommandBuffer {create_command_buffer(mDevice.get(), mCommandPool)},

      mImageAvailableSemaphore {create_semaphore(mDevice.get())},
      mRenderFinishedSemaphore {create_semaphore(mDevice.get())},
      mInFlightFence {create_signaled_fence(mDevice.get())},

      mAllocator {mInstance.get(), mGPU, mDevice.get()}
{
  mSwapchain.create_framebuffers(mRenderPass.get());
}

VulkanBackend::~VulkanBackend()
{
  vkDestroyFence(mDevice.get(), mInFlightFence, nullptr);
  vkDestroySemaphore(mDevice.get(), mRenderFinishedSemaphore, nullptr);
  vkDestroySemaphore(mDevice.get(), mImageAvailableSemaphore, nullptr);

  vkDestroyCommandPool(mDevice.get(), mCommandPool, nullptr);
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

void VulkanBackend::on_quit()
{
  vkDeviceWaitIdle(mDevice.get());
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

void VulkanBackend::begin_frame()
{
  // Wait until the previous frame has finished
  wait_and_then_reset_fence(mDevice.get(), mInFlightFence);

  // Acquire an image from the swapchain
  mSwapchain.acquire_next_image(mImageAvailableSemaphore);

  reset_command_buffer(mCommandBuffer);
  begin_command_buffer(mCommandBuffer);
}

void VulkanBackend::end_frame()
{
  GRAVEL_VK_CALL(vkEndCommandBuffer(mCommandBuffer), "[VK] Could not end command buffer");

  mDevice.submit_rendering_commands(mCommandBuffer,
                                    mImageAvailableSemaphore,
                                    mRenderFinishedSemaphore,
                                    mInFlightFence);

  mDevice.present_swapchain_image(mSwapchain.get(),
                                  mSwapchain.get_image_index(),
                                  mRenderFinishedSemaphore);
}

void VulkanBackend::render_scene(const Scene& scene,
                                 const Vec2& framebuffer_size,
                                 Dispatcher& dispatcher)
{
  // TODO resize swap chain images if necessary

  const auto swapchain_image_extent = mSwapchain.get_image_extent();

  mRenderPass.begin(mCommandBuffer,
                    mSwapchain.get_current_framebuffer(),
                    swapchain_image_extent);

  vkCmdBindPipeline(mCommandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    mShadingPipeline.get());

  set_viewport(mCommandBuffer, swapchain_image_extent);
  set_scissor(mCommandBuffer, VkOffset2D {0, 0}, swapchain_image_extent);

  vkCmdEndRenderPass(mCommandBuffer);
}

auto VulkanBackend::get_primary_framebuffer_handle() -> void*
{
  VkFramebuffer framebuffer = mSwapchain.get_current_framebuffer();
  return static_cast<void*>(framebuffer);
}

}  // namespace gravel::vlk
