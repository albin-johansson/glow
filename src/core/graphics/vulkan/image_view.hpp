#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"

namespace gravel::vlk {

class ImageView final {
 public:
  GRAVEL_DELETE_COPY(ImageView);

  ImageView(VkImage image, VkFormat image_format);

  ~ImageView() noexcept;

  ImageView(ImageView&& other) noexcept;

  auto operator=(ImageView&& other) noexcept -> ImageView&;

  [[nodiscard]] auto get() -> VkImageView { return mImageView; }

 private:
  VkImageView mImageView {VK_NULL_HANDLE};

  void dispose() noexcept;
};

}  // namespace gravel::vlk
