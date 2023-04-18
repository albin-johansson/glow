#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "graphics/vulkan/image/image.hpp"
#include "graphics/vulkan/image/image_view.hpp"

namespace glow::vk {

/// Wraps a 2D image and an associated image view.
class Texture2D final {
 public:
  GLOW_DELETE_COPY(Texture2D);
  GLOW_DEFAULT_MOVE(Texture2D);

  Texture2D(const VkExtent2D& extent,
            VkFormat format,
            VkImageUsageFlags usage,
            uint32 mip_levels,
            VkImageAspectFlags aspects,
            VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);

  [[nodiscard]] auto get_image() -> Image& { return mImage; }
  [[nodiscard]] auto get_view() -> VkImageView { return mView.get(); }

 private:
  Image mImage;
  ImageViewPtr mView;
};

}  // namespace glow::vk
