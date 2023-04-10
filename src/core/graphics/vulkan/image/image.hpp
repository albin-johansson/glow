#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/type/maybe.hpp"
#include "common/type/path.hpp"

namespace gravel::vk {

class Image final {
 public:
  GRAVEL_DELETE_COPY(Image);

  /// Creates an empty GPU image.
  ///
  /// \details
  /// The image is created with the `VK_IMAGE_USAGE_TRANSFER_DST` flag combined with the
  /// provided image usage flags. Put the pixel data into a staging buffer, and use
  /// the `copy_from_buffer` function to upload the data to the image.
  ///
  /// \param type the kind of image to create, e.g. `VK_IMAGE_TYPE_2D`.
  /// \param extent the size of the image (use depth 1 for 2D images).
  /// \param format format the image pixel format, e.g. `VK_FORMAT_R8G8B8A8_SRGB`.
  /// \param usage image usage mask, e.g. `VK_IMAGE_USAGE_SAMPLED_BIT`.
  explicit Image(VkImageType type,
                 VkExtent3D extent,
                 VkFormat format,
                 VkImageUsageFlags usage);

  ~Image() noexcept;

  Image(Image&& other) noexcept;

  auto operator=(Image&& other) noexcept -> Image&;

  /// Changes the layout of the image, which is initially `VK_IMAGE_LAYOUT_UNDEFINED`.
  void change_layout(VkImageLayout new_layout);

  /// Uploads pixel data from a buffer into the image.
  void copy_from_buffer(VkBuffer buffer);

  [[nodiscard]] auto get() noexcept -> VkImage { return mImage; }
  [[nodiscard]] auto get_extent() const noexcept -> const VkExtent3D& { return mExtent; }
  [[nodiscard]] auto get_format() const noexcept -> VkFormat { return mFormat; }
  [[nodiscard]] auto get_layout() const noexcept -> VkImageLayout { return mLayout; }

 private:
  VkImage mImage {VK_NULL_HANDLE};
  VmaAllocation mAllocation {VK_NULL_HANDLE};
  VkExtent3D mExtent {};
  VkFormat mFormat {};
  VkImageLayout mLayout {VK_IMAGE_LAYOUT_UNDEFINED};

  void dispose() noexcept;
};

/// Creates a 2D image with the pixel data of the image the specified file path.
[[nodiscard]] auto load_image_2d(const Path& path,
                                 VkFormat format,
                                 VkImageUsageFlags usage) -> Maybe<Image>;

}  // namespace gravel::vk
