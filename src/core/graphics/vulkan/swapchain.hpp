#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/maybe.hpp"
#include "common/type/memory.hpp"
#include "common/type/vector.hpp"
#include "graphics/vulkan/image/framebuffer.hpp"
#include "graphics/vulkan/image/image_view.hpp"
#include "graphics/vulkan/image/texture.hpp"

namespace glow::vk {

struct SwapchainDeleter final {
  void operator()(VkSwapchainKHR swapchain) noexcept;
};

using SwapchainPtr = Unique<VkSwapchainKHR_T, SwapchainDeleter>;

/// Represents a bundle of framebuffers used to present rendering results to the screen.
class Swapchain final {
 public:
  GLOW_DELETE_COPY(Swapchain);
  GLOW_DEFAULT_MOVE(Swapchain);

  /// Initializes the swapchain resources, except for the framebuffers.
  Swapchain();

  /// Recreates the swapchain resources.
  void recreate(VkRenderPass render_pass);

  /// Creates (or recreates) framebuffers for the swapchain images.
  void create_framebuffers(VkRenderPass render_pass);

  /// Attempts to acquire the next image from the available swapchain images.
  auto acquire_next_image(VkSemaphore semaphore) -> VkResult;

  /// Presents the current swapchain image to the presentation queue.
  auto present_image(VkSemaphore render_finished_semaphore) -> VkResult;

  /// Returns the framebuffer associated with the current swapchain image.
  [[nodiscard]] auto get_current_framebuffer() -> VkFramebuffer;

  /// Returns the total number of images in the swapchain.
  [[nodiscard]] auto get_image_count() const -> uint32;

  /// Returns the format used by swapchain images.
  [[nodiscard]] auto get_image_format() const -> VkFormat { return mImageFormat; }

  /// Returns the size of the swapchain images.
  [[nodiscard]] auto get_image_extent() const -> VkExtent2D { return mImageExtent; }

  [[nodiscard]] auto get() -> VkSwapchainKHR { return mSwapchain.get(); }

 private:
  SwapchainPtr mSwapchain;
  VkExtent2D mImageExtent {};
  VkFormat mImageFormat {};
  Vector<VkImage> mImages;
  Vector<ImageViewPtr> mImageViews;
  Vector<FramebufferPtr> mFramebuffers;
  uint32 mImageIndex {0};
  Maybe<Texture2D> mDepthTexture;

  void create_swapchain(VkSwapchainKHR old_swapchain = VK_NULL_HANDLE);
  void create_image_views();
  void create_depth_buffer();
};

}  // namespace glow::vk
