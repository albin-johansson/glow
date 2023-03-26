#pragma once

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/vector.hpp"

namespace gravel::vlk {

class Swapchain final {
 public:
  GRAVEL_DELETE_COPY(Swapchain);
  GRAVEL_DELETE_MOVE(Swapchain);

  Swapchain(SDL_Window* window,
            VkPhysicalDevice gpu,
            VkDevice device,
            VkSurfaceKHR surface);

  ~Swapchain();

  void create_framebuffers(VkRenderPass render_pass);

  void acquire_next_image(VkSemaphore semaphore);

  [[nodiscard]] auto get_current_framebuffer() -> VkFramebuffer;

  [[nodiscard]] auto get() -> VkSwapchainKHR { return mSwapchain; }
  [[nodiscard]] auto get_image_format() const -> VkFormat { return mImageFormat; }
  [[nodiscard]] auto get_image_extent() const -> VkExtent2D { return mImageExtent; }
  [[nodiscard]] auto get_image_index() const -> uint32 { return mImageIndex; }

 private:
  VkDevice mDevice {VK_NULL_HANDLE};
  VkSwapchainKHR mSwapchain {VK_NULL_HANDLE};
  VkExtent2D mImageExtent {};
  VkFormat mImageFormat {};
  Vector<VkImage> mImages;
  Vector<VkImageView> mImageViews;
  Vector<VkFramebuffer> mFramebuffers;
  uint32 mImageIndex {};

  void destroy_framebuffers();

  void create_image_views();
};

}  // namespace gravel::vlk
