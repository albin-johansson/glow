#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"

namespace gravel::vk {

GRAVEL_FORWARD_DECLARE_C(Image);

class ImageView final {
 public:
  GRAVEL_DELETE_COPY(ImageView);

  ImageView(VkImage image,
            VkFormat image_format,
            VkImageViewType type,
            VkImageAspectFlags aspects,
            uint32 mip_levels = 1);

  ImageView(Image& image, VkImageViewType type, VkImageAspectFlags aspects);

  ~ImageView() noexcept;

  ImageView(ImageView&& other) noexcept;

  auto operator=(ImageView&& other) noexcept -> ImageView&;

  [[nodiscard]] auto get() -> VkImageView { return mImageView; }

 private:
  VkImageView mImageView {VK_NULL_HANDLE};

  void dispose() noexcept;
};

}  // namespace gravel::vk
