#pragma once

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/vector.hpp"
#include "engine/backend.hpp"
#include "graphics/vulkan/allocator.hpp"
#include "graphics/vulkan/device.hpp"
#include "graphics/vulkan/instance.hpp"
#include "graphics/vulkan/render_pass.hpp"
#include "graphics/vulkan/shading_pipeline.hpp"
#include "graphics/vulkan/surface.hpp"
#include "graphics/vulkan/swapchain.hpp"

namespace gravel::vlk {

inline constexpr usize kMaxFramesInFlight = 2;

class VulkanBackend final : public Backend {
 public:
  GRAVEL_DELETE_COPY(VulkanBackend);
  GRAVEL_DELETE_MOVE(VulkanBackend);

  explicit VulkanBackend(SDL_Window* window);

  ~VulkanBackend() override;

  void stop() override;

  void on_init(Scene& scene) override;

  void on_quit() override;

  void on_event(const SDL_Event& event) override;

  void begin_frame() override;

  void end_frame() override;

  void render_scene(const Scene& scene,
                    const Vec2& framebuffer_size,
                    Dispatcher& dispatcher) override;

  void set_environment_texture(Scene& scene, const Path& path) override;

  void load_model(Scene& scene, const Path& path) override;

  [[nodiscard]] auto get_primary_framebuffer_handle() -> void* override;

  [[nodiscard]] auto should_quit() const -> bool override { return mQuit; }

 private:
  Instance mInstance;
  Surface mSurface;
  VkPhysicalDevice mGPU {VK_NULL_HANDLE};
  Device mDevice;
  Swapchain mSwapchain;
  RenderPass mRenderPass;
  VkPipelineCache mPipelineCache {VK_NULL_HANDLE};
  ShadingPipeline mShadingPipeline;
  VkCommandPool mCommandPool {VK_NULL_HANDLE};

  Vector<VkCommandBuffer> mCommandBuffers;
  Vector<VkSemaphore> mImageAvailableSemaphores;
  Vector<VkSemaphore> mRenderFinishedSemaphores;
  Vector<VkFence> mInFlightFences;
  usize mFrameIndex {0};

  Allocator mAllocator;

  bool mQuit {false};

  void prepare_imgui_for_vulkan();
};

}  // namespace gravel::vlk
