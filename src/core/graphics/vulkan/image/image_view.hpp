#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/memory.hpp"

namespace glow::vk {

GLOW_FORWARD_DECLARE_C(Image);

struct ImageViewDeleter final {
  void operator()(VkImageView image_view) noexcept;
};

using ImageViewPtr = Unique<VkImageView_T, ImageViewDeleter>;

/// Creates an image view.
///
/// \param image the associated image.
/// \param image_format the format used by the associated image.
/// \param type the type of the image view, e.g. `VK_IMAGE_VIEW_TYPE_2D`.
/// \param aspects bitmask of image aspects accessible by the view.
/// \param mip_levels the amount of mipmap levels accessible by the view.
/// \return an automatically managed image view.
[[nodiscard]] auto create_image_view(VkImage image,
                                     VkFormat image_format,
                                     VkImageViewType type,
                                     VkImageAspectFlags aspects,
                                     uint32 mip_levels = 1) -> ImageViewPtr;

/// Creates an image view.
///
/// \param image the associated image.
/// \param type the type of the image view, e.g. `VK_IMAGE_VIEW_TYPE_2D`.
/// \param aspects bitmask of image aspects accessible by the view.
/// \return an automatically managed image view.
[[nodiscard]] auto create_image_view(Image& image,
                                     VkImageViewType type,
                                     VkImageAspectFlags aspects) -> ImageViewPtr;

}  // namespace glow::vk
