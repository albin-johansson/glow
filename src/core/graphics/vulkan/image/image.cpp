#include "image.hpp"

#include <algorithm>  // max
#include <cmath>      // floor, log2

#include <fmt/chrono.h>
#include <spdlog/spdlog.h>
#include <vk_mem_alloc.h>

#include "common/debug/assert.hpp"
#include "common/debug/error.hpp"
#include "common/type/chrono.hpp"
#include "graphics/vulkan/buffer.hpp"
#include "graphics/vulkan/cmd/command_buffer.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util/vk_call.hpp"
#include "io/texture_loader.hpp"

namespace gravel::vk {
namespace {

void transition_image_layout(VkCommandBuffer command_buffer,
                             VkImage image,
                             const VkImageLayout old_layout,
                             const VkImageLayout new_layout,
                             const uint32 level_count,
                             const uint32 base_level)
{
  VkAccessFlags src_access = 0;
  VkAccessFlags dst_access = 0;
  VkPipelineStageFlags src_stage = 0;
  VkPipelineStageFlags dst_stage = 0;

  if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
      new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    src_access = 0;
    dst_access = VK_ACCESS_TRANSFER_WRITE_BIT;

    src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  }
  else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
           new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    src_access = VK_ACCESS_TRANSFER_WRITE_BIT;
    dst_access = VK_ACCESS_SHADER_READ_BIT;

    src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  }
  else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL &&
           new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    src_access = VK_ACCESS_TRANSFER_READ_BIT;
    dst_access = VK_ACCESS_SHADER_READ_BIT;

    src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  }
  else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
           new_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
    src_access = VK_ACCESS_TRANSFER_WRITE_BIT;
    dst_access = VK_ACCESS_TRANSFER_READ_BIT;

    src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  }
  else {
    throw Error {"[VK] Unsupported image layout transition"};
  }

  const VkImageMemoryBarrier image_memory_barrier {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .pNext = nullptr,

      .srcAccessMask = src_access,
      .dstAccessMask = dst_access,

      .oldLayout = old_layout,
      .newLayout = new_layout,

      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

      .image = image,

      .subresourceRange =
          VkImageSubresourceRange {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .baseMipLevel = base_level,
              .levelCount = level_count,
              .baseArrayLayer = 0,
              .layerCount = 1,
          },
  };

  vkCmdPipelineBarrier(command_buffer,
                       src_stage,
                       dst_stage,
                       0,
                       0,
                       nullptr,
                       0,
                       nullptr,
                       1,
                       &image_memory_barrier);
}

}  // namespace

Image::Image(const VkImageType type,
             const VkExtent3D extent,
             const VkFormat format,
             const VkImageUsageFlags usage)
    : mExtent {extent},
      mFormat {format},
      mMipLevels {1 + static_cast<uint32>(
                          std::floor(std::log2(std::max(extent.width, extent.height))))}
{
  GRAVEL_ASSERT(mLayout == VK_IMAGE_LAYOUT_UNDEFINED);
  GRAVEL_ASSERT(mFormat != VK_FORMAT_UNDEFINED);

  const VkImageCreateInfo image_info {
      .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,

      .imageType = type,
      .format = mFormat,
      .extent = mExtent,

      .mipLevels = mMipLevels,
      .arrayLayers = 1,

      .samples = VK_SAMPLE_COUNT_1_BIT,
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .usage = usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,

      .queueFamilyIndexCount = 0,
      .initialLayout = mLayout,
  };

  const VmaAllocationCreateInfo allocation_info {
      .flags = 0,
      .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
      .requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
      .preferredFlags = 0,
      .memoryTypeBits = 0,
      .pool = nullptr,
      .pUserData = nullptr,
      .priority = 0.0f,
  };

  GRAVEL_VK_CALL(vmaCreateImage(get_allocator(),
                                &image_info,
                                &allocation_info,
                                &mImage,
                                &mAllocation,
                                nullptr),
                 "[VK] Could not create image");
}

Image::~Image() noexcept
{
  dispose();
}

void Image::dispose() noexcept
{
  if (mImage != VK_NULL_HANDLE) {
    vmaDestroyImage(get_allocator(), mImage, mAllocation);
  }
}

Image::Image(Image&& other) noexcept
    : mImage {other.mImage},
      mAllocation {other.mAllocation},
      mExtent {other.mExtent},
      mFormat {other.mFormat},
      mLayout {other.mLayout},
      mMipLevels {other.mMipLevels}
{
  other.mImage = VK_NULL_HANDLE;
  other.mAllocation = VK_NULL_HANDLE;
}

auto Image::operator=(Image&& other) noexcept -> Image&
{
  if (this != &other) {
    dispose();

    mImage = other.mImage;
    mAllocation = other.mAllocation;
    mExtent = other.mExtent;
    mFormat = other.mFormat;
    mLayout = other.mLayout;
    mMipLevels = other.mMipLevels;

    other.mImage = VK_NULL_HANDLE;
    other.mAllocation = VK_NULL_HANDLE;
  }

  return *this;
}

void Image::change_layout(const VkImageLayout new_layout)
{
  execute_immediately([=, this](VkCommandBuffer cmd_buffer) {
    transition_image_layout(cmd_buffer, mImage, mLayout, new_layout, mMipLevels, 0);
    mLayout = new_layout;
  });
}

void Image::copy_from_buffer(VkBuffer buffer)
{
  execute_immediately([=, this](VkCommandBuffer cmd_buffer) {
    const VkBufferImageCopy region {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource =
            VkImageSubresourceLayers {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        .imageOffset = VkOffset3D {0, 0, 0},
        .imageExtent = mExtent,
    };

    vkCmdCopyBufferToImage(cmd_buffer, buffer, mImage, mLayout, 1, &region);
  });
}

void Image::generate_mipmaps()
{
  GRAVEL_ASSERT(mLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  execute_immediately([this](VkCommandBuffer cmd_buffer) {
    auto mip_width = static_cast<int32>(mExtent.width);
    auto mip_height = static_cast<int32>(mExtent.height);

    for (uint32 level = 1; level < mMipLevels; ++level) {
      const uint32 base_level = level - 1;

      transition_image_layout(cmd_buffer,
                              mImage,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                              1,
                              base_level);

      VkImageBlit blit {};
      blit.srcOffsets[0] = {0, 0, 0};
      blit.srcOffsets[1] = {mip_width, mip_height, 1};

      blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      blit.srcSubresource.mipLevel = base_level;
      blit.srcSubresource.baseArrayLayer = 0;
      blit.srcSubresource.layerCount = 1;

      blit.dstOffsets[0] = {0, 0, 0};
      blit.dstOffsets[1] = {(mip_width > 1) ? (mip_width / 2) : 1,
                            (mip_height > 1) ? (mip_height / 2) : 1,
                            1};

      blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      blit.dstSubresource.mipLevel = level;
      blit.dstSubresource.baseArrayLayer = 0;
      blit.dstSubresource.layerCount = 1;

      vkCmdBlitImage(cmd_buffer,
                     mImage,
                     VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                     mImage,
                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                     1,
                     &blit,
                     VK_FILTER_LINEAR);

      transition_image_layout(cmd_buffer,
                              mImage,
                              VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                              1,
                              base_level);

      if (mip_width > 1) {
        mip_width /= 2;
      }

      if (mip_height > 1) {
        mip_height /= 2;
      }
    }

    // Transitions the last mipmap image to the optimal shader read layout
    transition_image_layout(cmd_buffer,
                            mImage,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                            1,
                            mMipLevels - 1);

    mLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  });
}

auto load_image_2d(const Path& path, const VkFormat format, const VkImageUsageFlags usage)
    -> Maybe<Image>
{
  const auto start_time = Clock::now();

  if (const auto data =
          load_texture_data(path, TextureFormat::Byte, TextureChannels::RGBA)) {
    const auto width = static_cast<uint32>(data->size.x);
    const auto height = static_cast<uint32>(data->size.y);

    const VkExtent3D image_extent {width, height, 1};
    const uint64 data_size = width * height * 4;

    auto staging_buffer = Buffer::staging(data_size, 0);
    staging_buffer.set_data(data->pixels.get(), data_size);

    Image image {VK_IMAGE_TYPE_2D, image_extent, format, usage};

    // Optimize layout for the buffer transfer, and copy data from staging buffer
    image.change_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    image.copy_from_buffer(staging_buffer.get());

    // Generate mipmaps
    image.generate_mipmaps();

    GRAVEL_ASSERT(image.get_layout() == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    GRAVEL_ASSERT(image.get_format() == format);

    const auto end_time = Clock::now();
    spdlog::debug("[VK] Loaded image in {}",
                  chrono::duration_cast<Milliseconds>(end_time - start_time));

    return image;
  }

  return kNothing;
}

}  // namespace gravel::vk
