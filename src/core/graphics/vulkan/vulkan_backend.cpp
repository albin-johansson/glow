#include "vulkan_backend.hpp"

#include <SDL2/SDL_vulkan.h>
#include <fmt/format.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

#include "common/debug/assert.hpp"
#include "graphics/camera.hpp"
#include "graphics/renderer_info.hpp"
#include "graphics/vulkan/cmd/commands.hpp"
#include "graphics/vulkan/command_buffer.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/physical_device.hpp"
#include "graphics/vulkan/pipeline.hpp"
#include "graphics/vulkan/synchronization.hpp"
#include "graphics/vulkan/util.hpp"
#include "scene/scene.hpp"
#include "scene/transform.hpp"

namespace gravel::vlk {
namespace {

[[nodiscard]] auto select_gpu() -> VkPhysicalDevice
{
  GRAVEL_ASSERT(get_instance() != VK_NULL_HANDLE);
  GRAVEL_ASSERT(get_surface() != VK_NULL_HANDLE);

  VkPhysicalDevice gpu = get_suitable_physical_device(get_instance(), get_surface());
  set_gpu(gpu);

  return gpu;
}

}  // namespace

VulkanBackend::VulkanBackend(SDL_Window* window)
    : mInstance {},
      mSurface {window},
      mGPU {select_gpu()},
      mDevice {mGPU, mSurface.get()},
      mAllocator {},
      mRenderPass {mDevice.get(), mSwapchain.get_image_format()},
      mPipelineCache {create_pipeline_cache(mDevice.get())},
      mShadingPipeline {mDevice.get(), mRenderPass.get(), mSwapchain.get_image_extent()},
      mCommandPool {create_command_pool(mDevice.get(), mGPU, mSurface.get())},
{

  for (usize index = 0; index < kMaxFramesInFlight; ++index) {
    auto& frame_data = mFrames.emplace_back();
    frame_data.command_buffer = mCommandPool.create_command_buffer();
  }

  mSwapchain.create_framebuffers(mRenderPass.get());
}

void VulkanBackend::stop()
{
  mQuit = true;
}

void VulkanBackend::on_init(Scene& scene)
{
  auto& vulkan_context = scene.add<VulkanContext>();
  vulkan_context.gpu = mGPU;
  vulkan_context.device = mDevice.get();
  vulkan_context.graphics_queue = mDevice.get_graphics_queue();
  vulkan_context.presentation_queue = mDevice.get_present_queue();
  vulkan_context.command_pool = mCommandPool;
  vulkan_context.allocator = mAllocator.get();

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
  auto& frame = mFrames.at(mFrameIndex);

  // Wait until the previous frame has finished
  frame.in_flight_fence.wait();

  // Acquire an image from the swapchain
  const auto acquire_image_result =
      mSwapchain.acquire_next_image(frame.image_available_semaphore.get());

  if (acquire_image_result == VK_ERROR_OUT_OF_DATE_KHR) {
    mSwapchain.recreate(mRenderPass.get());
    return kFailure;
  }
  else if (acquire_image_result != VK_SUCCESS &&
           acquire_image_result != VK_SUBOPTIMAL_KHR) {
    throw Error {"[VK] Could not acquire next swapchain image"};
  }

  // The fence is only reset when we submit useful work
  frame.in_flight_fence.reset();

  reset_command_buffer(frame.command_buffer);
  begin_command_buffer(frame.command_buffer);


  return kSuccess;
}

void VulkanBackend::end_frame()
{
  auto& frame = mFrames.at(mFrameIndex);


  vkCmdEndRenderPass(frame.command_buffer);
  end_command_buffer(frame.command_buffer);

  mDevice.submit_rendering_commands(frame.command_buffer,
                                    frame.image_available_semaphore.get(),
                                    frame.render_finished_semaphore.get(),
                                    frame.in_flight_fence.get());

  const auto present_result =
      mDevice.present_swapchain_image(mSwapchain.get(),
                                      mSwapchain.get_image_index(),
                                      frame.render_finished_semaphore.get());

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
  auto& frame = mFrames.at(mFrameIndex);

  // TODO resize swap chain images if necessary

  const auto swapchain_image_extent = mSwapchain.get_image_extent();

  mRenderPass.begin(frame.command_buffer,
                    mSwapchain.get_current_framebuffer(),
                    swapchain_image_extent);

  mShadingPipeline.bind(frame.command_buffer, mFrameIndex);

  cmd::set_viewport(frame.command_buffer, swapchain_image_extent);
  cmd::set_scissor(frame.command_buffer, VkOffset2D {0, 0}, swapchain_image_extent);

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
