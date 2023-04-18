#include "texture.hpp"

namespace glow::vk {

Texture2D::Texture2D(const VkExtent2D& extent,
                     const VkFormat format,
                     const VkImageUsageFlags usage,
                     const uint32 mip_levels,
                     const VkImageAspectFlags aspects,
                     const VkSampleCountFlagBits samples)
    : mImage {VK_IMAGE_TYPE_2D,
              VkExtent3D {extent.width, extent.height, 1},
              format,
              usage,
              mip_levels,
              samples},
      mView {create_image_view(mImage, VK_IMAGE_VIEW_TYPE_2D, aspects)}
{
}

}  // namespace glow::vk
