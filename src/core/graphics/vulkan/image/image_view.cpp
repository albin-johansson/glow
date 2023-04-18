#include "image_view.hpp"

#include "common/debug/assert.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/image/image.hpp"
#include "graphics/vulkan/util/vk_call.hpp"

namespace glow::vk {

void ImageViewDeleter::operator()(VkImageView image_view) noexcept
{
  vkDestroyImageView(get_device(), image_view, nullptr);
}

auto create_image_view(VkImage image,
                       const VkFormat image_format,
                       const VkImageViewType type,
                       const VkImageAspectFlags aspects,
                       const uint32 mip_levels) -> ImageViewPtr
{
  GLOW_ASSERT(image != VK_NULL_HANDLE);
  GLOW_ASSERT(image_format != VK_FORMAT_UNDEFINED);

  const VkComponentMapping component_mapping {
      .r = VK_COMPONENT_SWIZZLE_IDENTITY,
      .g = VK_COMPONENT_SWIZZLE_IDENTITY,
      .b = VK_COMPONENT_SWIZZLE_IDENTITY,
      .a = VK_COMPONENT_SWIZZLE_IDENTITY,
  };

  const VkImageSubresourceRange subresource_range {
      .aspectMask = aspects,
      .baseMipLevel = 0,
      .levelCount = mip_levels,
      .baseArrayLayer = 0,
      .layerCount = 1,
  };

  const VkImageViewCreateInfo create_info {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .image = image,
      .viewType = type,
      .format = image_format,
      .components = component_mapping,
      .subresourceRange = subresource_range,
  };

  VkImageView image_view = VK_NULL_HANDLE;
  GLOW_VK_CALL(vkCreateImageView(get_device(), &create_info, nullptr, &image_view),
               "[VK] Could not create image view");

  return ImageViewPtr {image_view};
}

auto create_image_view(Image& image,
                       const VkImageViewType type,
                       const VkImageAspectFlags aspects) -> ImageViewPtr
{
  return create_image_view(image.get(),
                           image.get_format(),
                           type,
                           aspects,
                           image.get_mip_levels());
}

}  // namespace glow::vk
