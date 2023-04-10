#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/maybe.hpp"
#include "common/type/vector.hpp"
#include "graphics/vulkan/framebuffer.hpp"
#include "graphics/vulkan/image.hpp"
#include "graphics/vulkan/image_view.hpp"

namespace gravel::vlk {

class Swapchain final {
 public:
  GRAVEL_DELETE_COPY(Swapchain);
  GRAVEL_DELETE_MOVE(Swapchain);

  Swapchain();

  ~Swapchain();

  void recreate(VkRenderPass render_pass);

  void create_framebuffers(VkRenderPass render_pass);

  auto acquire_next_image(VkSemaphore semaphore) -> VkResult;

  /// Presents the current swapchain image to the presentation queue.
  auto present_image(VkSemaphore render_finished_semaphore) -> VkResult;

  [[nodiscard]] auto get_current_framebuffer() -> Framebuffer&;

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
  Vector<Framebuffer> mFramebuffers;
  uint32 mImageIndex {0};

  Maybe<Image> mDepthImage;
  Maybe<ImageView> mDepthImageView;

  void destroy_swapchain();

  void create_swapchain();
  void create_image_views();
  void create_depth_buffer();
};

}  // namespace gravel::vlk
