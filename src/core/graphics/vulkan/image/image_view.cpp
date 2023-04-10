#include "image_view.hpp"

#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util/vk_call.hpp"

namespace gravel::vk {

ImageView::ImageView(VkImage image,
                     const VkFormat image_format,
                     const VkImageViewType type,
                     const VkImageAspectFlags aspects)
{
  const VkImageViewCreateInfo create_info {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,

      .image = image,
      .viewType = type,
      .format = image_format,
      .components =
          VkComponentMapping {
              .r = VK_COMPONENT_SWIZZLE_IDENTITY,
              .g = VK_COMPONENT_SWIZZLE_IDENTITY,
              .b = VK_COMPONENT_SWIZZLE_IDENTITY,
              .a = VK_COMPONENT_SWIZZLE_IDENTITY,
          },
      .subresourceRange =
          VkImageSubresourceRange {
              .aspectMask = aspects,
              .baseMipLevel = 0,
              .levelCount = 1,
              .baseArrayLayer = 0,
              .layerCount = 1,
          },
  };

  GRAVEL_VK_CALL(vkCreateImageView(get_device(), &create_info, nullptr, &mImageView),
                 "[VK] Could not create image view");
}

ImageView::~ImageView() noexcept
{
  dispose();
}

void ImageView::dispose() noexcept
{
  if (mImageView != VK_NULL_HANDLE) {
    vkDestroyImageView(get_device(), mImageView, nullptr);
  }
}

ImageView::ImageView(ImageView&& other) noexcept
    : mImageView {other.mImageView}
{
  other.mImageView = VK_NULL_HANDLE;
}

auto ImageView::operator=(ImageView&& other) noexcept -> ImageView&
{
  if (this != &other) {
    dispose();

    mImageView = other.mImageView;
    other.mImageView = VK_NULL_HANDLE;
  }

  return *this;
}

}  // namespace gravel::vk
