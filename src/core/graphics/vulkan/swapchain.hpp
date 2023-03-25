#pragma once

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "common/predef.hpp"
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

  [[nodiscard]] auto get_image_format() const -> VkFormat { return mImageFormat; }
  [[nodiscard]] auto get_image_extent() const -> VkExtent2D { return mImageExtent; }

 private:
  VkDevice mDevice {VK_NULL_HANDLE};
  VkSwapchainKHR mSwapchain {VK_NULL_HANDLE};
  VkExtent2D mImageExtent {};
  VkFormat mImageFormat {};
  Vector<VkImage> mImages;
  Vector<VkImageView> mImageViews;

  void create_image_views();
};

}  // namespace gravel::vlk
