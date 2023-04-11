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
#include "graphics/vulkan/debug_messenger.hpp"
#include "graphics/vulkan/device.hpp"
#include "graphics/vulkan/image/sampler.hpp"
#include "graphics/vulkan/instance.hpp"
#include "graphics/vulkan/managed.hpp"
#include "graphics/vulkan/model.hpp"
#include "graphics/vulkan/pipeline/descriptor_pool.hpp"
#include "graphics/vulkan/pipeline/pipeline_builder.hpp"
#include "graphics/vulkan/pipeline/pipeline_cache.hpp"
#include "graphics/vulkan/render_pass.hpp"
#include "graphics/vulkan/shader_buffers.hpp"
#include "graphics/vulkan/surface.hpp"
#include "graphics/vulkan/swapchain/swapchain.hpp"
#include "graphics/vulkan/sync/fence.hpp"
#include "graphics/vulkan/sync/semaphore.hpp"
#include "scene/transform.hpp"
#include "util/arrays.hpp"

namespace gravel::vk {

inline constexpr VkDescriptorPoolSize kDescriptorPoolSizes[] {
    {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 256},
    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 256},
};

/// Stores resources required to allow for multiple frames in flight at the same time.
struct FrameData final {
  VkCommandBuffer command_buffer {VK_NULL_HANDLE};

  Semaphore image_available_semaphore {create_semaphore()};
  Semaphore render_finished_semaphore {create_semaphore()};
  Fence in_flight_fence {create_fence(true)};

  Buffer static_matrix_ubo {Buffer::uniform(sizeof(StaticMatrices))};
  Buffer material_ubo {Buffer::uniform(sizeof(MaterialBuffer))};

  DescriptorPool descriptor_pool {2'048,
                                  kDescriptorPoolSizes,
                                  array_length(kDescriptorPoolSizes)};
};

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

  auto begin_frame() -> Result override;

  void end_frame() override;

  void render_scene(const Scene& scene,
                    const Vec2& framebuffer_size,
                    Dispatcher& dispatcher) override;

  void set_environment_texture(Scene& scene, const Path& path) override;

  void load_model(Scene& scene, const Path& path) override;

  [[nodiscard]] auto get_primary_framebuffer_handle() -> void* override;

  [[nodiscard]] auto should_quit() const -> bool override { return mQuit; }

 private:
  Instance mInstance {create_instance()};
  DebugMessenger mDebugMessenger {kDebugBuild ? create_debug_messenger() : nullptr};
  Surface mSurface {create_surface()};
  VkPhysicalDevice mGPU {VK_NULL_HANDLE};
  Device mDevice {create_device()};
  Allocator mAllocator;
  Swapchain mSwapchain;
  RenderPass mRenderPass;
  Sampler mSampler {create_sampler()};
  PipelineCache mPipelineCache {create_pipeline_cache()};

  DescriptorSetLayoutBuilder mDescriptorSetLayoutBuilder;
  PipelineLayoutBuilder mPipelineLayoutBuilder;
  PipelineBuilder mPipelineBuilder;

  DescriptorSetLayout mShadingDescriptorSetLayout;
  PipelineLayout mShadingPipelineLayout;
  Pipeline mShadingPipeline;

  CommandPool mCommandPool;

  Vector<FrameData> mFrames;
  usize mFrameIndex {0};

  MaterialBuffer mMaterialBuffer;
  StaticMatrices mStaticMatrices;

  bool mQuit {false};
  bool mResizedFramebuffer : 1 {false};

  void create_shading_pipeline();
  void create_frame_data();
  void prepare_imgui_for_vulkan();

  void update_static_matrix_buffer(const Camera& camera,
                                   const Transform& camera_transform);

  void update_material_buffer(const Material& material);

  void push_static_matrix_descriptor();

  void render_model(const Scene& scene, const Transform& transform, const Model& model);

  /// Submits rendering commands to the graphics queue.
  void submit_commands();

  void present_image();
};

}  // namespace gravel::vk
