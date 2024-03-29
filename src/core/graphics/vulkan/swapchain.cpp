#include "swapchain.hpp"

#include <algorithm>  // clamp, min
#include <array>      // size
#include <limits>     // numeric_limits
#include <utility>    // move

#include <imgui_impl_vulkan.h>
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

#include "common/debug/assert.hpp"
#include "common/debug/error.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/physical_device.hpp"
#include "graphics/vulkan/util/size.hpp"
#include "graphics/vulkan/util/vk_call.hpp"
#include "init/window.hpp"

namespace glow::vk {
namespace {

[[nodiscard]] auto _pick_swap_surface_format(const Vector<VkSurfaceFormatKHR>& formats)
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

[[nodiscard]] auto _pick_swap_present_mode(const Vector<VkPresentModeKHR>& modes)
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

[[nodiscard]] auto _pick_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities)
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

[[nodiscard]] auto _get_swapchain_images(VkSwapchainKHR swapchain) -> Vector<VkImage>
{
  uint32 image_count = 0;
  vkGetSwapchainImagesKHR(get_device(), swapchain, &image_count, nullptr);

  Vector<VkImage> images;
  images.resize(image_count);
  vkGetSwapchainImagesKHR(get_device(), swapchain, &image_count, images.data());

  return images;
}

}  // namespace

void SwapchainDeleter::operator()(VkSwapchainKHR swapchain) noexcept
{
  vkDestroySwapchainKHR(get_device(), swapchain, nullptr);
}

Swapchain::Swapchain()
{
  create_swapchain();
  create_image_views();
  create_depth_buffer();
}

void Swapchain::create_image_views()
{
  GLOW_ASSERT(mImageViews.empty());
  mImageViews.reserve(mImages.size());

  for (VkImage image : mImages) {
    mImageViews.push_back(create_image_view(image,
                                            mImageFormat,
                                            VK_IMAGE_VIEW_TYPE_2D,
                                            VK_IMAGE_ASPECT_COLOR_BIT));
  }
}

void Swapchain::create_depth_buffer()
{
  mDepthTexture.emplace(mImageExtent,
                        VK_FORMAT_D32_SFLOAT_S8_UINT,
                        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                        1,
                        VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
                        VK_SAMPLE_COUNT_1_BIT);
}

void Swapchain::recreate(VkRenderPass render_pass)
{
  int window_width = 0;
  int window_height = 0;
  SDL_GetWindowSizeInPixels(get_window(), &window_width, &window_height);

  while (window_width == 0 || window_height == 0) {
    SDL_GetWindowSizeInPixels(get_window(), &window_width, &window_height);
    SDL_WaitEvent(nullptr);
  }

  // Avoid touching resources that may still be in use
  vkDeviceWaitIdle(get_device());

  // Destroy existing resources in the right order
  mFramebuffers.clear();
  mImageViews.clear();
  mDepthTexture.reset();

  auto old_swapchain = std::move(mSwapchain);
  create_swapchain(old_swapchain.get());
  old_swapchain.reset();

  create_image_views();
  create_depth_buffer();
  create_framebuffers(render_pass);
}

void Swapchain::create_swapchain(VkSwapchainKHR old_swapchain)
{
  const auto queue_family_indices = get_queue_family_indices(get_gpu(), get_surface());
  const auto swapchain_support = get_swapchain_support(get_gpu(), get_surface());

  const uint32 queue_family_indices_arr[] = {
      queue_family_indices.graphics_family.value(),
      queue_family_indices.present_family.value(),
  };

  const auto surface_format = _pick_swap_surface_format(swapchain_support.formats);
  const auto present_mode = _pick_swap_present_mode(swapchain_support.present_modes);

  mImageFormat = surface_format.format;
  mImageExtent = _pick_swap_extent(swapchain_support.capabilities);

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

      .oldSwapchain = old_swapchain,
  };

  if (queue_family_indices.graphics_family != queue_family_indices.present_family) {
    spdlog::debug("[VK] Using distinct graphics and presentation queues");

    // In this case we opt for concurrent mode, so that images can be (easily) used across
    // multiple queue families, since the graphics and present queues are distinct.
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = std::size(queue_family_indices_arr);
    create_info.pQueueFamilyIndices = queue_family_indices_arr;
  }
  else {
    spdlog::debug("[VK] Queue supports both graphics and presentation operations");

    // Here we can safely opt for the exclusive mode, leading to the best performance.
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = nullptr;
  }

  mImageExtent = create_info.imageExtent;
  mImageFormat = create_info.imageFormat;

  VkSwapchainKHR swapchain = VK_NULL_HANDLE;
  GLOW_VK_CALL(vkCreateSwapchainKHR(get_device(), &create_info, nullptr, &swapchain),
               "[VK] Could not create swapchain");
  mSwapchain.reset(swapchain);

  mImages = _get_swapchain_images(mSwapchain.get());

  spdlog::debug(
      "[VK] Created swapchain with {} images using format {}, presented using {}",
      mImages.size(),
      magic_enum::enum_name(mImageFormat),
      magic_enum::enum_name(create_info.presentMode));
}

void Swapchain::create_framebuffers(VkRenderPass render_pass)
{
  GLOW_ASSERT(render_pass != VK_NULL_HANDLE);
  GLOW_ASSERT(mDepthTexture.has_value());

  mFramebuffers.clear();
  mFramebuffers.reserve(mImageViews.size());

  for (auto& image_view : mImageViews) {
    mFramebuffers.push_back(create_framebuffer(render_pass,
                                               image_view.get(),
                                               mDepthTexture->get_view(),
                                               mImageExtent));
  }
}

auto Swapchain::acquire_next_image(VkSemaphore semaphore) -> VkResult
{
  return vkAcquireNextImageKHR(get_device(),
                               mSwapchain.get(),
                               UINT64_MAX,
                               semaphore,
                               VK_NULL_HANDLE,
                               &mImageIndex);
}

auto Swapchain::present_image(VkSemaphore render_finished_semaphore) -> VkResult
{
  VkSwapchainKHR swapchain = mSwapchain.get();

  const VkPresentInfoKHR present_info {
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .pNext = nullptr,

      // Wait on the render_finished_semaphore before presentation
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &render_finished_semaphore,

      // Target swapchain image
      .swapchainCount = 1,
      .pSwapchains = &swapchain,
      .pImageIndices = &mImageIndex,

      .pResults = nullptr,
  };

  return vkQueuePresentKHR(get_presentation_queue(), &present_info);
}

auto Swapchain::get_current_framebuffer() -> VkFramebuffer
{
  GLOW_ASSERT(!mFramebuffers.empty());
  GLOW_ASSERT(!mImageViews.empty());
  GLOW_ASSERT(!mImages.empty());
  GLOW_ASSERT(mDepthTexture.has_value());

  return mFramebuffers.at(static_cast<usize>(mImageIndex)).get();
}

auto Swapchain::get_image_count() const -> uint32
{
  return u32_size(mFramebuffers);
}

}  // namespace glow::vk
