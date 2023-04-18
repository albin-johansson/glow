#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/maybe.hpp"
#include "common/type/memory.hpp"
#include "common/type/path.hpp"

namespace glow::vk {

/// Helper type for wrapping a VkImage/VmaAllocation pair.
class AllocatedImage final {
 public:
  GLOW_DELETE_COPY(AllocatedImage);

  VkImage image {VK_NULL_HANDLE};
  VmaAllocation allocation {VK_NULL_HANDLE};

  AllocatedImage() noexcept = default;

  ~AllocatedImage() noexcept;

  AllocatedImage(AllocatedImage&& other) noexcept;

  auto operator=(AllocatedImage&& other) noexcept -> AllocatedImage&;

 private:
  void destroy() noexcept;
};

class Image final {
 public:
  GLOW_DELETE_COPY(Image);
  GLOW_DEFAULT_MOVE(Image);

  /// Creates an empty GPU image.
  ///
  /// \details
  /// The image is created with the `VK_IMAGE_USAGE_TRANSFER_SRC` and
  /// `VK_IMAGE_USAGE_TRANSFER_DST` flags combined with the provided image usage flags.
  /// Put the pixel data into a staging buffer, and use the `copy_from_buffer` function to
  /// upload the data to the image.
  ///
  /// \param type the kind of image to create, e.g. `VK_IMAGE_TYPE_2D`.
  /// \param extent the size of the image (use depth 1 for 2D images).
  /// \param format format the image pixel format, e.g. `VK_FORMAT_R8G8B8A8_SRGB`.
  /// \param usage image usage mask, e.g. `VK_IMAGE_USAGE_SAMPLED_BIT`.
  /// \param mip_levels the desired amount of mip levels.
  /// \param samples the amount of samples to store per texel.
  Image(VkImageType type,
        VkExtent3D extent,
        VkFormat format,
        VkImageUsageFlags usage,
        uint32 mip_levels,
        VkSampleCountFlagBits samples);

  /// Changes the layout of the image, which is initially `VK_IMAGE_LAYOUT_UNDEFINED`.
  void change_layout(VkImageLayout new_layout);

  /// Uploads pixel data from a buffer into the image.
  void copy_from_buffer(VkBuffer buffer);

  void generate_mipmaps();

  [[nodiscard]] static auto max_mip_levels(const VkExtent3D extent) -> uint32;

  [[nodiscard]] auto get() noexcept -> VkImage { return mData.image; }
  [[nodiscard]] auto get_extent() const noexcept -> const VkExtent3D& { return mExtent; }
  [[nodiscard]] auto get_format() const noexcept -> VkFormat { return mFormat; }
  [[nodiscard]] auto get_layout() const noexcept -> VkImageLayout { return mLayout; }
  [[nodiscard]] auto get_samples() const noexcept -> VkSampleCountFlagBits
  {
    return mSamples;
  }
  [[nodiscard]] auto get_mip_levels() const noexcept -> uint32 { return mMipLevels; }

 private:
  AllocatedImage mData;
  VkExtent3D mExtent {};
  VkFormat mFormat {VK_FORMAT_UNDEFINED};
  VkImageLayout mLayout {VK_IMAGE_LAYOUT_UNDEFINED};
  VkSampleCountFlagBits mSamples {};
  uint32 mMipLevels {1};
};

/// Creates a 2D image with the pixel data of the image the specified file path.
[[nodiscard]] auto load_image_2d(const Path& path,
                                 VkFormat format,
                                 VkImageUsageFlags usage) -> Maybe<Image>;

/// Creates a 2D image with the specified pixel data.
[[nodiscard]] auto load_image_2d(const void* pixels,
                                 uint64 data_size,
                                 VkExtent2D size,
                                 VkFormat format,
                                 VkImageUsageFlags usage) -> Maybe<Image>;

}  // namespace glow::vk
