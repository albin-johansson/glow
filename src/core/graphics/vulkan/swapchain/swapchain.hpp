#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/maybe.hpp"
#include "common/type/vector.hpp"
#include "graphics/vulkan/image/framebuffer.hpp"
#include "graphics/vulkan/image/image.hpp"
#include "graphics/vulkan/image/image_view.hpp"

namespace glow::vk {

class Swapchain final {
 public:
  GLOW_DELETE_COPY(Swapchain);
  GLOW_DELETE_MOVE(Swapchain);

  Swapchain();

  ~Swapchain();

  void recreate(VkRenderPass render_pass);

  void create_framebuffers(VkRenderPass render_pass);

  /// Attempts to acquire the next image from the available swapchain images.
  auto acquire_next_image(VkSemaphore semaphore) -> VkResult;

  /// Presents the current swapchain image to the presentation queue.
  auto present_image(VkSemaphore render_finished_semaphore) -> VkResult;

  [[nodiscard]] auto get_current_framebuffer() -> VkFramebuffer;

  [[nodiscard]] auto get_image_count() const -> uint32;

  [[nodiscard]] auto get() -> VkSwapchainKHR { return mSwapchain; }
  [[nodiscard]] auto get_image_format() const -> VkFormat { return mImageFormat; }
  [[nodiscard]] auto get_image_extent() const -> VkExtent2D { return mImageExtent; }
  [[nodiscard]] auto get_image_index() const -> uint32 { return mImageIndex; }

 private:
  VkSwapchainKHR mSwapchain {VK_NULL_HANDLE};
  VkExtent2D mImageExtent {};
  VkFormat mImageFormat {};

  Vector<VkImage> mImages;
  Vector<ImageView> mImageViews;
  Vector<FramebufferPtr> mFramebuffers;
  uint32 mImageIndex {0};

  Maybe<Image> mDepthImage;
  Maybe<ImageView> mDepthImageView;

  void create_swapchain(VkSwapchainKHR old_swapchain = VK_NULL_HANDLE);
  void create_image_views();
  void create_depth_buffer();
};

}  // namespace glow::vk
