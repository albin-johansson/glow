#include "swapchain.hpp"

#include <algorithm>  // clamp, min
#include <limits>     // numeric_limits

#include <spdlog/spdlog.h>

#include "common/debug/error.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/framebuffer.hpp"
#include "graphics/vulkan/physical_device.hpp"
#include "graphics/vulkan/util.hpp"
#include "init/window.hpp"
#include "util/arrays.hpp"

namespace gravel::vlk {
namespace {

[[nodiscard]] auto pick_swap_surface_format(const Vector<VkSurfaceFormatKHR>& formats)
    -> VkSurfaceFormatKHR
{
  for (const auto& format : formats) {
    if ((format.format == VK_FORMAT_B8G8R8A8_SRGB ||
         format.format == VK_FORMAT_R8G8B8A8_SRGB) &&
        format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return format;
    }
  }

  // Just pick the first one if no format matched our wishes
  return formats.at(0);
}

[[nodiscard]] auto pick_swap_present_mode(const Vector<VkPresentModeKHR>& modes)
    -> VkPresentModeKHR
{
  for (const auto mode : modes) {
    // Use triple buffering if available
    if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return mode;
    }
  }

  // Use the only present mode guaranteed to be supported as a fallback
  return VK_PRESENT_MODE_FIFO_KHR;
}

[[nodiscard]] auto pick_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities)
    -> VkExtent2D
{
  if (capabilities.currentExtent.width != std::numeric_limits<uint32>::max()) {
    return capabilities.currentExtent;
  }
  else {
    int width {};
    int height {};
    SDL_GetWindowSizeInPixels(get_window(), &width, &height);

    VkExtent2D extent;
    extent.width = std::clamp(static_cast<uint32>(width),
                              capabilities.minImageExtent.width,
                              capabilities.maxImageExtent.width);
    extent.height = std::clamp(static_cast<uint32>(height),
                               capabilities.minImageExtent.height,
                               capabilities.maxImageExtent.height);

    return extent;
  }
}

[[nodiscard]] auto get_swapchain_images(VkDevice device, VkSwapchainKHR swapchain)
    -> Vector<VkImage>
{
  uint32 image_count = 0;
  vkGetSwapchainImagesKHR(device, swapchain, &image_count, nullptr);

  Vector<VkImage> images;
  images.resize(image_count);
  vkGetSwapchainImagesKHR(device, swapchain, &image_count, images.data());

  return images;
}

}  // namespace

Swapchain::Swapchain()
{
  create_swapchain();
  create_image_views();
}

Swapchain::~Swapchain()
{
  destroy_framebuffers();
  destroy_image_views();
  destroy_swapchain();
}

void Swapchain::destroy_framebuffers()
{
  for (VkFramebuffer framebuffer : mFramebuffers) {
    vkDestroyFramebuffer(get_device(), framebuffer, nullptr);
  }

  mFramebuffers.clear();
}

void Swapchain::destroy_image_views()
{
  for (VkImageView image_view : mImageViews) {
    vkDestroyImageView(get_device(), image_view, nullptr);
  }
}

void Swapchain::destroy_swapchain()
{
  vkDestroySwapchainKHR(get_device(), mSwapchain, nullptr);
}

void Swapchain::create_image_views()
{
  mImageViews.resize(mImages.size());

  usize index = 0;
  for (const auto& image : mImages) {
    const VkImageViewCreateInfo create_info {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,

        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = mImageFormat,
        .components =
            VkComponentMapping {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
        .subresourceRange =
            VkImageSubresourceRange {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
    };

    auto& image_view = mImageViews.at(index);
    GRAVEL_VK_CALL(vkCreateImageView(get_device(), &create_info, nullptr, &image_view),
                   "[VK] Could not create swapchain image view");

    ++index;
  }
}

void Swapchain::recreate(VkRenderPass render_pass)
{
  spdlog::debug("[VK] Recreating swapchain");

  int width = 0;
  int height = 0;
  SDL_GetWindowSizeInPixels(get_window(), &width, &height);

  while (width == 0 || height == 0) {
    SDL_GetWindowSizeInPixels(get_window(), &width, &height);
    SDL_WaitEvent(nullptr);
  }

  // Avoid touching resources that may still be in use
  vkDeviceWaitIdle(get_device());

  // Destroy existing resources
  destroy_framebuffers();
  destroy_image_views();
  destroy_swapchain();

  // Recreate the resources
  create_swapchain();
  create_image_views();
  create_framebuffers(render_pass);
}

void Swapchain::create_swapchain()
{
  const auto queue_family_indices = get_queue_family_indices(get_gpu(), get_surface());
  const auto swapchain_support = get_swapchain_support(get_gpu(), get_surface());

  const uint32 queue_family_indices_arr[] = {
      queue_family_indices.graphics_family.value(),
      queue_family_indices.present_family.value(),
  };

  const auto surface_format = pick_swap_surface_format(swapchain_support.formats);
  const auto present_mode = pick_swap_present_mode(swapchain_support.present_modes);

  mImageFormat = surface_format.format;
  mImageExtent = pick_swap_extent(swapchain_support.capabilities);

  uint32 min_image_count = swapchain_support.capabilities.minImageCount + 1;
  if (swapchain_support.capabilities.maxImageCount > 0) {
    min_image_count =
        std::min(min_image_count, swapchain_support.capabilities.maxImageCount);
  }

  VkSwapchainCreateInfoKHR create_info {
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,

      .surface = get_surface(),
      .minImageCount = min_image_count,

      .imageFormat = mImageFormat,
      .imageColorSpace = surface_format.colorSpace,
      .imageExtent = mImageExtent,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,

      .preTransform = swapchain_support.capabilities.currentTransform,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = present_mode,
      .clipped = VK_TRUE,

      .oldSwapchain = VK_NULL_HANDLE,
  };

  if (queue_family_indices.graphics_family != queue_family_indices.present_family) {
    // In this case we opt for concurrent mode, so that images can be (easily) used across
    // multiple queue families, since the graphics and present queues are distinct.
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = array_length(queue_family_indices_arr);
    create_info.pQueueFamilyIndices = queue_family_indices_arr;
  }
  else {
    // Here we can safely opt for the exclusive mode, leading to the best performance.
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = nullptr;
  }

  mImageExtent = create_info.imageExtent;
  mImageFormat = create_info.imageFormat;

  GRAVEL_VK_CALL(vkCreateSwapchainKHR(get_device(), &create_info, nullptr, &mSwapchain),
                 "[VK] Could not create swapchain");

  mImages = get_swapchain_images(get_device(), mSwapchain);
}

void Swapchain::create_framebuffers(VkRenderPass render_pass)
{
  if (!mFramebuffers.empty()) {
    destroy_framebuffers();
  }

  mFramebuffers.clear();
  mFramebuffers.reserve(mImageViews.size());

  for (VkImageView image_view : mImageViews) {
    VkFramebuffer framebuffer =
        create_framebuffer(get_device(), render_pass, image_view, mImageExtent);
    mFramebuffers.push_back(framebuffer);
  }
}

auto Swapchain::acquire_next_image(VkSemaphore semaphore) -> VkResult
{
  return vkAcquireNextImageKHR(get_device(),
                               mSwapchain,
                               UINT64_MAX,
                               semaphore,
                               VK_NULL_HANDLE,
                               &mImageIndex);
}

auto Swapchain::get_current_framebuffer() -> VkFramebuffer
{
  return mFramebuffers.at(static_cast<usize>(mImageIndex));
}

}  // namespace gravel::vlk
