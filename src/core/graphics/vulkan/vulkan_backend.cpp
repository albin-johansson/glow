#include "vulkan_backend.hpp"

#include <SDL2/SDL_vulkan.h>
#include <fmt/format.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

#include "graphics/camera.hpp"
#include "graphics/renderer_info.hpp"
#include "graphics/vulkan/command_buffer.hpp"
#include "graphics/vulkan/physical_device.hpp"
#include "graphics/vulkan/pipeline.hpp"
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
      mPipelineCache {create_pipeline_cache(mDevice.get())},
      mShadingPipeline {mDevice.get(), mRenderPass.get(), mSwapchain.get_image_extent()},
      mCommandPool {create_command_pool(mDevice.get(), mGPU, mSurface.get())},
      mAllocator {mInstance.get(), mGPU, mDevice.get()}
{
  mCommandBuffers =
      create_command_buffers(mDevice.get(), mCommandPool, kMaxFramesInFlight);

  mImageAvailableSemaphores.reserve(kMaxFramesInFlight);
  mRenderFinishedSemaphores.reserve(kMaxFramesInFlight);
  mInFlightFences.reserve(kMaxFramesInFlight);

  for (usize index = 0; index < kMaxFramesInFlight; ++index) {
    mImageAvailableSemaphores.push_back(create_semaphore(mDevice.get()));
    mRenderFinishedSemaphores.push_back(create_semaphore(mDevice.get()));
    mInFlightFences.push_back(create_signaled_fence(mDevice.get()));
  }

  mSwapchain.create_framebuffers(mRenderPass.get());
}

VulkanBackend::~VulkanBackend()
{
  VkDevice device = mDevice.get();

  for (usize index = 0; index < kMaxFramesInFlight; ++index) {
    vkDestroyFence(device, mInFlightFences.at(index), nullptr);
    vkDestroySemaphore(device, mRenderFinishedSemaphores.at(index), nullptr);
    vkDestroySemaphore(device, mImageAvailableSemaphores.at(index), nullptr);
  }

  vkDestroyCommandPool(device, mCommandPool, nullptr);
  vkDestroyPipelineCache(device, mPipelineCache, nullptr);
}

void VulkanBackend::stop()
{
  mQuit = true;
}

void VulkanBackend::on_init(Scene& scene)
{
  auto& allocator_context = scene.add<AllocatorContext>();
  allocator_context.allocator = mAllocator.get();

  prepare_imgui_for_vulkan();

  VkPhysicalDeviceProperties gpu_properties {};
  vkGetPhysicalDeviceProperties(mGPU, &gpu_properties);

  auto& renderer_info = scene.get<RendererInfo>();
  renderer_info.api = get_api_version(mGPU);
  renderer_info.renderer = gpu_properties.deviceName;
  renderer_info.vendor = "N/A";
  renderer_info.version = get_driver_version(mGPU);

  const auto camera_entity = make_camera(scene, "Camera", Vec3 {0, 0, 2}, Vec3 {0, 0, 1});

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
  else if (event.type == SDL_WINDOWEVENT) {
    if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
        event.window.event == SDL_WINDOWEVENT_RESIZED) {
      mResizedFramebuffer = true;
    }
  }
}

auto VulkanBackend::begin_frame() -> Result
{
  // Wait until the previous frame has finished
  wait_fence(mDevice.get(), mInFlightFences.at(mFrameIndex));

  // Acquire an image from the swapchain
  const auto acquire_image_result =
      mSwapchain.acquire_next_image(mImageAvailableSemaphores.at(mFrameIndex));

  if (acquire_image_result == VK_ERROR_OUT_OF_DATE_KHR) {
    mSwapchain.recreate(mRenderPass.get());
    return kFailure;
  }
  else if (acquire_image_result != VK_SUCCESS &&
           acquire_image_result != VK_SUBOPTIMAL_KHR) {
    throw Error {"[VK] Could not acquire next swapchain image"};
  }

  // The fence is only reset when we submit useful work
  reset_fence(mDevice.get(), mInFlightFences.at(mFrameIndex));

  VkCommandBuffer command_buffer = mCommandBuffers.at(mFrameIndex);
  reset_command_buffer(command_buffer);
  begin_command_buffer(command_buffer);
  return kSuccess;
}

void VulkanBackend::end_frame()
{
  VkCommandBuffer command_buffer = mCommandBuffers.at(mFrameIndex);


  vkCmdEndRenderPass(command_buffer);
  GRAVEL_VK_CALL(vkEndCommandBuffer(command_buffer), "[VK] Could not end command buffer");

  mDevice.submit_rendering_commands(command_buffer,
                                    mImageAvailableSemaphores.at(mFrameIndex),
                                    mRenderFinishedSemaphores.at(mFrameIndex),
                                    mInFlightFences.at(mFrameIndex));

  const auto present_result =
      mDevice.present_swapchain_image(mSwapchain.get(),
                                      mSwapchain.get_image_index(),
                                      mRenderFinishedSemaphores.at(mFrameIndex));

  if (present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR ||
      mResizedFramebuffer) {
    mResizedFramebuffer = false;
    mSwapchain.recreate(mRenderPass.get());
  }
  else if (present_result != VK_SUCCESS) {
    throw Error {"[VK] Could not present swapchain image"};
  }

  mFrameIndex = (mFrameIndex + 1) % kMaxFramesInFlight;
}

void VulkanBackend::render_scene(const Scene& scene,
                                 const Vec2& framebuffer_size,
                                 Dispatcher& dispatcher)
{
  // TODO resize swap chain images if necessary

  VkCommandBuffer command_buffer = mCommandBuffers.at(mFrameIndex);
  const auto swapchain_image_extent = mSwapchain.get_image_extent();

  mRenderPass.begin(command_buffer,
                    mSwapchain.get_current_framebuffer(),
                    swapchain_image_extent);

  vkCmdBindPipeline(command_buffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    mShadingPipeline.get());

  set_viewport(command_buffer, swapchain_image_extent);
  set_scissor(command_buffer, VkOffset2D {0, 0}, swapchain_image_extent);

  // TODO render models

}

void VulkanBackend::set_environment_texture([[maybe_unused]] Scene& scene,
                                            [[maybe_unused]] const Path& path)
{
  // TODO
}

void VulkanBackend::load_model([[maybe_unused]] Scene& scene,
                               [[maybe_unused]] const Path& path)
{
  // TODO
}

auto VulkanBackend::get_primary_framebuffer_handle() -> void*
{
  VkFramebuffer framebuffer = mSwapchain.get_current_framebuffer();
  return static_cast<void*>(framebuffer);
}

}  // namespace gravel::vlk
