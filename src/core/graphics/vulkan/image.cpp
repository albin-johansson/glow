#include "image.hpp"

#include <vk_mem_alloc.h>

#include "common/debug/error.hpp"
#include "graphics/vulkan/buffer.hpp"
#include "graphics/vulkan/cmd/command_buffer.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util.hpp"
#include "io/texture_loader.hpp"

namespace gravel::vlk {
namespace {

void transition_image_layout(VkCommandBuffer command_buffer,
                             VkImage image,
                             const VkImageLayout old_layout,
                             const VkImageLayout new_layout)
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
              .baseMipLevel = 0,
              .levelCount = 1,
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
    : mExtent {extent}
{
  const VkImageCreateInfo image_info {
      .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,

      .imageType = type,
      .format = format,
      .extent = mExtent,

      .mipLevels = 1,
      .arrayLayers = 1,

      .samples = VK_SAMPLE_COUNT_1_BIT,
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .usage = usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
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
      mLayout {other.mLayout}
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
    mLayout = other.mLayout;

    other.mImage = VK_NULL_HANDLE;
    other.mAllocation = VK_NULL_HANDLE;
  }

  return *this;
}

void Image::change_layout(VkImageLayout new_layout)
{
  VkCommandBuffer cmd_buffer = record_one_time_commands();
  transition_image_layout(cmd_buffer, mImage, mLayout, new_layout);
  execute_one_time_commands(cmd_buffer);

  mLayout = new_layout;
}

void Image::copy_from_buffer(VkBuffer buffer)
{
  VkCommandBuffer cmd_buffer = record_one_time_commands();

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

  execute_one_time_commands(cmd_buffer);
}

auto load_image_2d(const Path& path, VkFormat format, VkImageUsageFlags usage)
    -> Maybe<Image>
{
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

    // Ensure image has optimal layout for being read from shaders
    image.change_layout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    return image;
  }

  return kNothing;
}

}  // namespace gravel::vlk
