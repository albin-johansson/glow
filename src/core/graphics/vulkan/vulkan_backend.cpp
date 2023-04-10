#include "vulkan_backend.hpp"

#include <fmt/format.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>
#include <spdlog/spdlog.h>

// This comment prevents moving the following include before <imgui.h>
#include <ImGuizmo.h>

#include "common/debug/assert.hpp"
#include "graphics/renderer_info.hpp"
#include "graphics/vulkan/cmd/command_buffer.hpp"
#include "graphics/vulkan/cmd/commands.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/image_cache.hpp"
#include "graphics/vulkan/physical_device.hpp"
#include "graphics/vulkan/pipeline/descriptor.hpp"
#include "graphics/vulkan/util.hpp"
#include "scene/scene.hpp"

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
    : mGPU {select_gpu()},
      mRenderPass {mSwapchain.get_image_format()},
      mPipelineBuilder {mPipelineCache.get()}
{
  create_shading_pipeline();
  create_frame_data();

  mSwapchain.create_framebuffers(mRenderPass.get());
}

VulkanBackend::~VulkanBackend() noexcept
{
  ImGui_ImplVulkan_Shutdown();
}

void VulkanBackend::create_shading_pipeline()
{
  mDescriptorSetLayoutBuilder.reset()
      .use_push_descriptors()
      .descriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
      .descriptor(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
      .descriptor(5,
                  VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                  VK_SHADER_STAGE_FRAGMENT_BIT);
  mShadingDescriptorSetLayout.reset(mDescriptorSetLayoutBuilder.build());

  mPipelineLayoutBuilder.reset()
      .descriptor_layout(mShadingDescriptorSetLayout.get())
      .push_constant(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Mat4));
  mShadingPipelineLayout.reset(mPipelineLayoutBuilder.build());

  mPipelineBuilder.reset()
      .shaders("assets/shaders/vk/shading.vert.spv", "assets/shaders/vk/shading.frag.spv")
      .rasterization(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT)
      .multisample(VK_SAMPLE_COUNT_1_BIT)
      .input_assembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
      .blending(false);
  mShadingPipeline.reset(
      mPipelineBuilder.build(mRenderPass.get(), mShadingPipelineLayout.get()));
}

void VulkanBackend::create_frame_data()
{
  for (usize index = 0; index < kMaxFramesInFlight; ++index) {
    auto& frame = mFrames.emplace_back();
    frame.command_buffer = mCommandPool.create_command_buffer();
  }
}

void VulkanBackend::stop()
{
  mQuit = true;
}

void VulkanBackend::on_init(Scene& scene)
{
  scene.add<ImageCache>();

  prepare_imgui_for_vulkan();

  VkPhysicalDevicePushDescriptorPropertiesKHR push_descriptor_properties {};
  push_descriptor_properties.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR;

  VkPhysicalDeviceProperties2 gpu_properties {};
  gpu_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
  gpu_properties.pNext = &push_descriptor_properties;
  vkGetPhysicalDeviceProperties2(mGPU, &gpu_properties);

  spdlog::debug("[VK] Max push constant size: {} bytes",
                gpu_properties.properties.limits.maxPushConstantsSize);
  spdlog::debug("[VK] Max sampler anisotropy: {}",
                gpu_properties.properties.limits.maxSamplerAnisotropy);
  spdlog::debug("[VK] Max bound descriptor sets: {}",
                gpu_properties.properties.limits.maxBoundDescriptorSets);
  spdlog::debug("[VK] Max memory allocation count: {}",
                gpu_properties.properties.limits.maxMemoryAllocationCount);
  spdlog::debug("[VK] Max uniform buffer range: {} bytes",
                gpu_properties.properties.limits.maxUniformBufferRange);
  spdlog::debug("[VK] Max push descriptors in descriptor set layout: {}",
                push_descriptor_properties.maxPushDescriptors);

  auto& renderer_info = scene.get<RendererInfo>();
  renderer_info.api = get_api_version(mGPU);
  renderer_info.renderer = gpu_properties.properties.deviceName;
  renderer_info.vendor = "N/A";
  renderer_info.version = get_driver_version(mGPU);

  const auto camera_entity =
      make_camera(scene, "Camera", Vec3 {0, 2, -5}, Vec3 {0, 0, 1});

  auto& camera_context = scene.get<CameraContext>();
  camera_context.active_camera = camera_entity;
}

void VulkanBackend::prepare_imgui_for_vulkan()
{
}

void VulkanBackend::on_quit()
{
  vkDeviceWaitIdle(mDevice.get());
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
  wait_fence(frame.in_flight_fence.get());

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
  reset_fence(frame.in_flight_fence.get());

  reset_command_buffer(frame.command_buffer);
  begin_command_buffer(frame.command_buffer);

  ImGui_ImplSDL2_NewFrame();
  ImGui_ImplVulkan_NewFrame();
  ImGui::NewFrame();
  ImGuizmo::BeginFrame();

  return kSuccess;
}

void VulkanBackend::render_scene(const Scene& scene,
                                 const Vec2& framebuffer_size,
                                 Dispatcher& dispatcher)
{
  auto& frame = mFrames.at(mFrameIndex);

  // TODO resize swap chain images if necessary

  const auto swapchain_image_extent = mSwapchain.get_image_extent();
  const auto aspect_ratio = framebuffer_size.x / framebuffer_size.y;

  if (!scene.has_active_camera()) {
    return;
  }

  const auto& [camera_entity, camera] = scene.get_active_camera();
  const auto& camera_transform = scene.get<Transform>(camera_entity);

  if (camera.aspect_ratio != aspect_ratio) {
    // dispatcher.enqueue<SetCameraAspectRatioEvent>(camera_entity, aspect_ratio);
  }

  update_static_matrix_buffer(camera, camera_transform);

  mRenderPass.begin(frame.command_buffer,
                    mSwapchain.get_current_framebuffer().get(),
                    swapchain_image_extent);

  cmd::set_viewport(frame.command_buffer, swapchain_image_extent);
  cmd::set_scissor(frame.command_buffer, VkOffset2D {0, 0}, swapchain_image_extent);

  vkCmdBindPipeline(frame.command_buffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    mShadingPipeline.get());

  push_static_matrix_descriptor();

  static Vector<VkWriteDescriptorSet> write_buffer;
  write_buffer.reserve(5);

  for (auto [entity, transform, model] : scene.each<Transform, Model>()) {
    const auto model_transform = transform.to_model_matrix();

    for (const auto& mesh : model.meshes) {
      write_buffer.clear();

      const auto& material = scene.get<Material>(mesh.material);
      const auto model_matrix = model_transform * mesh.transform;

      vkCmdPushConstants(frame.command_buffer,
                         mShadingPipelineLayout.get(),
                         VK_SHADER_STAGE_VERTEX_BIT,
                         0,
                         sizeof model_matrix,
                         &model_matrix);

      update_material_buffer(material);

      const VkDescriptorBufferInfo material_buffer_info {
          .buffer = frame.material_ubo.get(),
          .offset = 0,
          .range = sizeof mMaterialBuffer,
      };

      const VkDescriptorImageInfo diffuse_image_info {
          .sampler = mSampler.get(),
          .imageView = material.diffuse_tex,
          .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      };

      // Update for the material buffer
      write_buffer.push_back(
          create_descriptor_buffer_write(1,
                                         VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                         &material_buffer_info));

      if (material.diffuse_tex != VK_NULL_HANDLE) {
        // Change the bound diffuse material texture
        write_buffer.push_back(
            create_descriptor_image_write(5,
                                          VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                          &diffuse_image_info));
      }

      push_descriptor_set(frame.command_buffer,
                          mShadingPipelineLayout.get(),
                          0,
                          static_cast<uint32>(write_buffer.size()),
                          write_buffer.data());

      mesh.vertex_buffer->bind_as_vertex_buffer(frame.command_buffer);
      mesh.index_buffer->bind_as_index_buffer(frame.command_buffer, VK_INDEX_TYPE_UINT32);

      vkCmdDrawIndexed(frame.command_buffer, mesh.index_count, 1, 0, 0, 0);
    }
  }

}

void VulkanBackend::end_frame()
{
  auto& frame = mFrames.at(mFrameIndex);

  ImGui::Render();
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), frame.command_buffer);

  vkCmdEndRenderPass(frame.command_buffer);
  end_command_buffer(frame.command_buffer);

  submit_commands();
  present_image();

  mFrameIndex = (mFrameIndex + 1) % kMaxFramesInFlight;
}

void VulkanBackend::submit_commands()
{
  auto& frame = mFrames.at(mFrameIndex);

  VkSemaphore image_available_semaphore = frame.image_available_semaphore.get();
  VkSemaphore render_finished_semaphore = frame.render_finished_semaphore.get();

  const VkPipelineStageFlags wait_dst_stage_mask =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  const VkSubmitInfo submit_info {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = nullptr,

      // Wait on the image_available_semaphore before command buffer execution
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &image_available_semaphore,
      .pWaitDstStageMask = &wait_dst_stage_mask,

      .commandBufferCount = 1,
      .pCommandBuffers = &frame.command_buffer,

      // Signal the render_finished_semaphore after command buffer execution
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = &render_finished_semaphore,
  };

  VkQueue queue = get_graphics_queue();
  GRAVEL_VK_CALL(vkQueueSubmit(queue, 1, &submit_info, frame.in_flight_fence.get()),
                 "[VK] Could not submit command buffer to graphics queue");
}

void VulkanBackend::present_image()
{
  auto& frame = mFrames.at(mFrameIndex);

  const auto present_result =
      mSwapchain.present_image(frame.render_finished_semaphore.get());

  if (present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR ||
      mResizedFramebuffer) {
    mResizedFramebuffer = false;
    mSwapchain.recreate(mRenderPass.get());
  }
  else if (present_result != VK_SUCCESS) {
    throw Error {"[VK] Could not present swapchain image"};
  }
}

void VulkanBackend::update_static_matrix_buffer(const Camera& camera,
                                                const Transform& camera_transform)
{
  mStaticMatrices.proj = to_projection_matrix(camera, GraphicsApi::Vulkan);
  mStaticMatrices.view = to_view_matrix(camera, camera_transform, GraphicsApi::Vulkan);
  mStaticMatrices.view_proj = mStaticMatrices.proj * mStaticMatrices.view;

  auto& frame = mFrames.at(mFrameIndex);
  frame.static_matrix_ubo.set_data(&mStaticMatrices, sizeof mStaticMatrices);
}

void VulkanBackend::update_material_buffer(const Material& material)
{
  mMaterialBuffer.ambient = Vec4 {material.ambient, 0};
  mMaterialBuffer.diffuse = Vec4 {material.diffuse, 0};
  mMaterialBuffer.specular = Vec4 {material.specular, 0};
  mMaterialBuffer.emission = Vec4 {material.emission, 0};

  mMaterialBuffer.has_ambient_tex = false;
  mMaterialBuffer.has_diffuse_tex = material.diffuse_tex != VK_NULL_HANDLE;
  mMaterialBuffer.has_specular_tex = material.specular_tex != VK_NULL_HANDLE;
  mMaterialBuffer.has_emission_tex = false;

  auto& frame = mFrames.at(mFrameIndex);
  frame.material_ubo.set_data(&mMaterialBuffer, sizeof mMaterialBuffer);
}

void VulkanBackend::push_static_matrix_descriptor()
{
  auto& frame = mFrames.at(mFrameIndex);

  const auto buffer_info =
      StaticMatrices::descriptor_buffer_info(frame.static_matrix_ubo.get());

  const VkWriteDescriptorSet writes[] {
      StaticMatrices::descriptor_set_write(VK_NULL_HANDLE, &buffer_info),
  };

  push_descriptor_set(frame.command_buffer,
                      mShadingPipelineLayout.get(),
                      0,
                      array_length(writes),
                      writes);
}

void VulkanBackend::set_environment_texture([[maybe_unused]] Scene& scene,
                                            [[maybe_unused]] const Path& path)
{
  // TODO
}

void VulkanBackend::load_model(Scene& scene, const Path& path)
{
  static int index = 0;

  const auto model_entity = scene.make_node(fmt::format("Model {}", index));
  assign_model(scene, model_entity, path);

  ++index;
}

auto VulkanBackend::get_primary_framebuffer_handle() -> void*
{
  // TODO need to use different render target framebuffer, not the actual one in swapchain
  //  VkFramebuffer framebuffer = mSwapchain.get_current_framebuffer();
  //  return static_cast<void*>(framebuffer);

  return nullptr;
}

}  // namespace gravel::vlk
