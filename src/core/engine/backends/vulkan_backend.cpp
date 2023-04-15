#include "vulkan_backend.hpp"

#include <cstddef>  // offsetof

#include <fmt/format.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>
#include <spdlog/spdlog.h>

// This comment prevents moving the following include before <imgui.h>
#include <ImGuizmo.h>

#include "common/debug/assert.hpp"
#include "graphics/renderer_info.hpp"
#include "graphics/vertex.hpp"
#include "graphics/vulkan/cmd/command_buffer.hpp"
#include "graphics/vulkan/cmd/commands.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/image/image_cache.hpp"
#include "graphics/vulkan/physical_device.hpp"
#include "graphics/vulkan/pipeline/descriptor.hpp"
#include "graphics/vulkan/queue.hpp"
#include "graphics/vulkan/util/constants.hpp"
#include "graphics/vulkan/util/size.hpp"
#include "graphics/vulkan/util/version.hpp"
#include "graphics/vulkan/util/vk_call.hpp"
#include "init/dear_imgui_vulkan.hpp"
#include "scene/scene.hpp"

namespace glow {
namespace {

[[nodiscard]] auto select_gpu() -> VkPhysicalDevice
{
  GRAVEL_ASSERT(vk::get_instance() != VK_NULL_HANDLE);
  GRAVEL_ASSERT(vk::get_surface() != VK_NULL_HANDLE);

  auto* gpu = vk::get_suitable_physical_device(vk::get_instance(), vk::get_surface());
  vk::set_gpu(gpu);

  return gpu;
}

[[nodiscard]] auto create_render_pass(const VkFormat swapchain_image_format)
    -> vk::RenderPassInfo
{
  vk::RenderPassBuilder builder;

  const auto external_subpass_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                                       VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  const auto main_subpass_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                                   VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

  const auto main_subpass_access =
      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  builder
      .attachment(swapchain_image_format,
                  VK_IMAGE_LAYOUT_UNDEFINED,
                  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
      .attachment(VK_FORMAT_D32_SFLOAT_S8_UINT,
                  VK_IMAGE_LAYOUT_UNDEFINED,
                  VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
      .begin_subpass()
      .color_attachment(0)
      .depth_attachment(1)
      .end_subpass()
      .dependency(VK_SUBPASS_EXTERNAL,
                  0,
                  external_subpass_stages,
                  main_subpass_stages,
                  0,
                  main_subpass_access);

  return builder.build();
}

}  // namespace

VulkanBackend::VulkanBackend()
    : mInstance {vk::create_instance()},
      mDebugMessenger {kDebugBuild ? vk::create_debug_messenger() : nullptr},
      mSurface {vk::create_surface()},
      mGPU {select_gpu()},
      mDevice {vk::create_device()},
      mAllocator {vk::create_allocator()},
      mSwapchain {},
      mRenderPassInfo {create_render_pass(mSwapchain.get_image_format())},
      mSampler {vk::create_sampler()},
      mPipelineCache {vk::create_pipeline_cache()},
      mImGuiData {}
{
  create_shading_pipeline();
  create_frame_data();

  mSwapchain.create_framebuffers(mRenderPassInfo.pass.get());
}

VulkanBackend::~VulkanBackend() noexcept
{
  ImGui_ImplVulkan_Shutdown();
}

void VulkanBackend::create_shading_pipeline()
{
  vk::DescriptorSetLayoutBuilder descriptor_set_layout_builder;
  descriptor_set_layout_builder  //
      .use_push_descriptors()
      .descriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
      .descriptor(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
      .descriptor(5,
                  VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                  VK_SHADER_STAGE_FRAGMENT_BIT);
  mShadingDescriptorSetLayout.reset(descriptor_set_layout_builder.build());

  vk::PipelineLayoutBuilder pipeline_layout_builder;
  pipeline_layout_builder  //
      .descriptor_set_layout(mShadingDescriptorSetLayout.get())
      .push_constant(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Mat4));
  mShadingPipelineLayout.reset(pipeline_layout_builder.build());

  vk::PipelineBuilder pipeline_builder {mPipelineCache.get()};
  pipeline_builder
      .shaders("assets/shaders/vk/shading.vert.spv", "assets/shaders/vk/shading.frag.spv")
      .vertex_input_binding(0, sizeof(Vertex))
      .vertex_attribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position))
      .vertex_attribute(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal))
      .vertex_attribute(0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, tex_coords))
      .rasterization(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT)
      .multisample(VK_SAMPLE_COUNT_1_BIT)
      .input_assembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
      .blending(false);
  mShadingPipeline.reset(
      pipeline_builder.build(mRenderPassInfo.pass.get(), mShadingPipelineLayout.get()));
}

void VulkanBackend::create_frame_data()
{
  for (usize index = 0; index < vk::kMaxFramesInFlight; ++index) {
    auto& frame = mFrames.emplace_back();
    frame.command_buffer = mCommandPool.allocate_command_buffer();
  }
}

void VulkanBackend::stop()
{
  mQuit = true;
}

void VulkanBackend::on_init(Scene& scene)
{
  vk::init_imgui(mImGuiData, mRenderPassInfo.pass.get(), mSwapchain.get_image_count());

  scene.add<vk::ImageCache>();

  VkPhysicalDeviceProperties2 gpu_properties {};
  gpu_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
  vkGetPhysicalDeviceProperties2(mGPU, &gpu_properties);

  auto& renderer_info = scene.get<RendererInfo>();
  renderer_info.api = vk::get_api_version(mGPU);
  renderer_info.renderer = gpu_properties.properties.deviceName;
  renderer_info.vendor = "N/A";
  renderer_info.version = vk::get_driver_version(mGPU);

  const auto camera_entity =
      make_camera(scene, "Camera", Vec3 {0, 2, -5}, Vec3 {0, 0, 1});

  auto& camera_context = scene.get<CameraContext>();
  camera_context.active_camera = camera_entity;
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

auto VulkanBackend::begin_frame(const Scene& scene) -> Result
{
  if (!scene.has_active_camera()) {
    return kFailure;
  }

  auto& frame = mFrames.at(mFrameIndex);

  // Wait until the previous frame has finished
  vk::wait_fence(frame.in_flight_fence.get());

  // Acquire an image from the swapchain
  const auto acquire_image_result =
      mSwapchain.acquire_next_image(frame.image_available_semaphore.get());

  if (acquire_image_result == VK_ERROR_OUT_OF_DATE_KHR) {
    spdlog::debug("[VK] Recreating outdated swapchain");
    mSwapchain.recreate(mRenderPassInfo.pass.get());
    return kFailure;
  }
  else if (acquire_image_result != VK_SUCCESS &&
           acquire_image_result != VK_SUBOPTIMAL_KHR) {
    throw Error {"[VK] Could not acquire next swapchain image"};
  }

  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
  ImGuizmo::BeginFrame();

  // The fence is only reset when we submit useful work
  vk::reset_fence(frame.in_flight_fence.get());

  vk::reset_command_buffer(frame.command_buffer);
  vk::begin_command_buffer(frame.command_buffer);

  vk::cmd_begin_render_pass(frame.command_buffer,
                            mRenderPassInfo,
                            mSwapchain.get_current_framebuffer().get(),
                            mSwapchain.get_image_extent());

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

  const auto& [camera_entity, camera] = scene.get_active_camera();
  const auto& camera_transform = scene.get<Transform>(camera_entity);

  if (camera.aspect_ratio != aspect_ratio) {
    // dispatcher.enqueue<SetCameraAspectRatioEvent>(camera_entity, aspect_ratio);
  }

  vkCmdBindPipeline(frame.command_buffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    mShadingPipeline.get());

  vk::cmd::set_viewport(frame.command_buffer, swapchain_image_extent);
  vk::cmd::set_scissor(frame.command_buffer, VkOffset2D {0, 0}, swapchain_image_extent);

  update_static_matrix_buffer(camera, camera_transform);
  push_static_matrix_descriptor();

  for (auto [entity, transform, model] : scene.each<Transform, vk::Model>()) {
    render_model(scene, transform, model);
  }
}

void VulkanBackend::render_model(const Scene& scene,
                                 const Transform& transform,
                                 const vk::Model& model)
{
  auto& frame = mFrames.at(mFrameIndex);

  static Vector<VkWriteDescriptorSet> write_buffer;
  write_buffer.reserve(5);

  const auto model_transform = transform.to_model_matrix();

  for (const auto& mesh : model.meshes) {
    write_buffer.clear();

    const auto& material = scene.get<vk::Material>(mesh.material);
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
        vk::create_descriptor_buffer_write(1,
                                           VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                           &material_buffer_info));

    if (material.diffuse_tex != VK_NULL_HANDLE) {
      // Change the bound diffuse material texture
      write_buffer.push_back(
          vk::create_descriptor_image_write(5,
                                            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                            &diffuse_image_info));
    }

    vk::push_descriptor_set(frame.command_buffer,
                            mShadingPipelineLayout.get(),
                            0,
                            vk::u32_size(write_buffer),
                            write_buffer.data());

    mesh.vertex_buffer->bind_as_vertex_buffer(frame.command_buffer);
    mesh.index_buffer->bind_as_index_buffer(frame.command_buffer, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(frame.command_buffer, mesh.index_count, 1, 0, 0, 0);
  }
}

void VulkanBackend::end_frame()
{
  auto& frame = mFrames.at(mFrameIndex);

  ImGui::Render();
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), frame.command_buffer);

  vkCmdEndRenderPass(frame.command_buffer);
  vk::end_command_buffer(frame.command_buffer);

  // Submits rendering commands to the graphics queue. Waits on the
  // image_available_semaphore before executing the command buffer, and signals
  // render_finished_semaphore and in_flight_fence after the commands are executed.
  vk::submit_to_queue(vk::get_graphics_queue(),
                      frame.command_buffer,
                      frame.image_available_semaphore.get(),
                      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                      frame.render_finished_semaphore.get(),
                      frame.in_flight_fence.get());

  // Presents the current framebuffer to the swapchain.
  present_image();

  // Move on to the next frame.
  mFrameIndex = (mFrameIndex + 1) % vk::kMaxFramesInFlight;
}

void VulkanBackend::present_image()
{
  auto& frame = mFrames.at(mFrameIndex);

  const auto present_result =
      mSwapchain.present_image(frame.render_finished_semaphore.get());

  if (present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR ||
      mResizedFramebuffer) {
    spdlog::debug("[VK] Recreating outdated or suboptimal swapchain");
    mResizedFramebuffer = false;
    mSwapchain.recreate(mRenderPassInfo.pass.get());
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

void VulkanBackend::update_material_buffer(const vk::Material& material)
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
      vk::StaticMatrices::descriptor_buffer_info(frame.static_matrix_ubo.get());

  const VkWriteDescriptorSet writes[] {
      vk::StaticMatrices::descriptor_set_write(VK_NULL_HANDLE, &buffer_info),
  };

  vk::push_descriptor_set(frame.command_buffer,
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
  vk::assign_model(scene, model_entity, path);

  ++index;
}

auto VulkanBackend::get_primary_framebuffer_handle() -> void*
{
  // TODO need to use different render target framebuffer, not the actual one in swapchain
  //  VkFramebuffer framebuffer = mSwapchain.get_current_framebuffer();
  //  return static_cast<void*>(framebuffer);

  return nullptr;
}

}  // namespace glow
