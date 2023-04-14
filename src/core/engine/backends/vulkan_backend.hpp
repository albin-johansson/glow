#pragma once

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/vector.hpp"
#include "engine/backend.hpp"
#include "graphics/camera.hpp"
#include "graphics/vulkan/allocator.hpp"
#include "graphics/vulkan/buffer.hpp"
#include "graphics/vulkan/cmd/command_pool.hpp"
#include "graphics/vulkan/device.hpp"
#include "graphics/vulkan/frame.hpp"
#include "graphics/vulkan/image/sampler.hpp"
#include "graphics/vulkan/imgui.hpp"
#include "graphics/vulkan/instance.hpp"
#include "graphics/vulkan/model.hpp"
#include "graphics/vulkan/pipeline/descriptor_pool.hpp"
#include "graphics/vulkan/pipeline/descriptor_set_layout.hpp"
#include "graphics/vulkan/pipeline/pipeline.hpp"
#include "graphics/vulkan/pipeline/pipeline_builder.hpp"
#include "graphics/vulkan/pipeline/pipeline_cache.hpp"
#include "graphics/vulkan/pipeline/pipeline_layout.hpp"
#include "graphics/vulkan/render_pass.hpp"
#include "graphics/vulkan/shader_buffers.hpp"
#include "graphics/vulkan/surface.hpp"
#include "graphics/vulkan/swapchain/swapchain.hpp"
#include "graphics/vulkan/sync/fence.hpp"
#include "graphics/vulkan/sync/semaphore.hpp"
#include "graphics/vulkan/util/debug_messenger.hpp"
#include "scene/transform.hpp"
#include "util/arrays.hpp"

namespace gravel {

class VulkanBackend final : public Backend {
 public:
  GRAVEL_DELETE_COPY(VulkanBackend);
  GRAVEL_DELETE_MOVE(VulkanBackend);

  VulkanBackend();

  ~VulkanBackend() noexcept override;

  void stop() override;

  void on_init(Scene& scene) override;

  void on_quit() override;

  void on_event(const SDL_Event& event) override;

  auto begin_frame(const Scene& scene) -> Result override;

  void end_frame() override;

  void render_scene(const Scene& scene,
                    const Vec2& framebuffer_size,
                    Dispatcher& dispatcher) override;

  void set_environment_texture(Scene& scene, const Path& path) override;

  void load_model(Scene& scene, const Path& path) override;

  [[nodiscard]] auto get_primary_framebuffer_handle() -> void* override;

  [[nodiscard]] auto should_quit() const -> bool override { return mQuit; }

 private:
  vk::Instance mInstance;
  vk::DebugMessenger mDebugMessenger;
  vk::Surface mSurface;
  VkPhysicalDevice mGPU;
  vk::Device mDevice;
  vk::Allocator mAllocator;
  vk::Swapchain mSwapchain;
  vk::RenderPass mRenderPass;
  vk::Sampler mSampler;
  vk::PipelineCache mPipelineCache;
  vk::ImGuiData mImGuiData;
  vk::CommandPool mCommandPool;
  vk::DescriptorSetLayoutBuilder mDescriptorSetLayoutBuilder;
  vk::PipelineLayoutBuilder mPipelineLayoutBuilder;
  vk::PipelineBuilder mPipelineBuilder;
  vk::DescriptorSetLayout mShadingDescriptorSetLayout;
  vk::PipelineLayout mShadingPipelineLayout;
  vk::Pipeline mShadingPipeline;
  Vector<vk::FrameData> mFrames;
  usize mFrameIndex {0};
  vk::MaterialBuffer mMaterialBuffer;
  vk::StaticMatrices mStaticMatrices;
  bool mQuit {false};
  bool mResizedFramebuffer : 1 {false};

  void create_shading_pipeline();
  void create_frame_data();

  void update_static_matrix_buffer(const Camera& camera,
                                   const Transform& camera_transform);

  void update_material_buffer(const vk::Material& material);

  void push_static_matrix_descriptor();

  void render_model(const Scene& scene,
                    const Transform& transform,
                    const vk::Model& model);

  /// Submits rendering commands to the graphics queue.
  void submit_commands();

  void present_image();
};

}  // namespace gravel
