#include "image_view.hpp"

#include "common/debug/assert.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/image/image.hpp"
#include "graphics/vulkan/util/vk_call.hpp"

namespace glow::vk {

ImageView::ImageView(VkImage image,
                     const VkFormat image_format,
                     const VkImageViewType type,
                     const VkImageAspectFlags aspects,
                     const uint32 mip_levels)
{
  GLOW_ASSERT(image_format != VK_FORMAT_UNDEFINED);

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
              .levelCount = mip_levels,
              .baseArrayLayer = 0,
              .layerCount = 1,
          },
  };

  GLOW_VK_CALL(vkCreateImageView(get_device(), &create_info, nullptr, &mImageView),
               "[VK] Could not create image view");
}

ImageView::ImageView(Image& image,
                     const VkImageViewType type,
                     const VkImageAspectFlags aspects)
    : ImageView {image.get(), image.get_format(), type, aspects, image.get_mip_levels()}
{
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

}  // namespace glow::vk
