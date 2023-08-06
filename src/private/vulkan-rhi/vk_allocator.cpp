/*
 * MIT License
 *
 * Copyright (c) 2023 Albin Johansson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "glow/vulkan-rhi/vk_allocator.hpp"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include "glow/core/foundation/error.hpp"

namespace glow {
namespace {

struct VmaAllocatorDeleter final {
  void operator()(VmaAllocator allocator) noexcept { vmaDestroyAllocator(allocator); }
};

using UniqueVmaAllocator = std::unique_ptr<VmaAllocator_T, VmaAllocatorDeleter>;

}  // namespace

struct VulkanAllocator::Data final {
  VkInstance instance {};
  VkDevice device {};
  VkPhysicalDevice gpu {};
  UniqueVmaAllocator allocator;
};

VulkanAllocator::VulkanAllocator(VkInstance instance,
                                 VkDevice device,
                                 VkPhysicalDevice gpu)
    : mData {std::make_unique<Data>()}
{
  VmaAllocatorCreateInfo allocator_info = {};
  allocator_info.physicalDevice = gpu;
  allocator_info.device = device;
  allocator_info.instance = instance;
  allocator_info.vulkanApiVersion = VK_API_VERSION_1_2;

  VmaAllocator allocator = VK_NULL_HANDLE;
  const auto result = vmaCreateAllocator(&allocator_info, &allocator);

  if (result != VK_SUCCESS) {
    throw Error {"Could not create Vulkan allocator"};
  }

  mData->allocator.reset(allocator);
}

VulkanAllocator::~VulkanAllocator() noexcept = default;

auto VulkanAllocator::make_buffer(const VmaAllocationCreateInfo& allocation_info,
                                  const VkBufferCreateInfo& buffer_info,
                                  VmaAllocation* out_allocation,
                                  VkBuffer* out_buffer) -> VkResult
{
  return vmaCreateBuffer(mData->allocator.get(),
                         &buffer_info,
                         &allocation_info,
                         out_buffer,
                         out_allocation,
                         nullptr);
}

auto VulkanAllocator::make_image(const VmaAllocationCreateInfo& allocation_info,
                                 const VkImageCreateInfo& image_info,
                                 VmaAllocation* out_allocation,
                                 VkImage* out_image) -> VkResult
{
  return vmaCreateImage(mData->allocator.get(),
                        &image_info,
                        &allocation_info,
                        out_image,
                        out_allocation,
                        nullptr);
}

void VulkanAllocator::destroy_buffer(VmaAllocation allocation, VkBuffer buffer)
{
  vmaDestroyBuffer(mData->allocator.get(), buffer, allocation);
}

void VulkanAllocator::destroy_image(VmaAllocation allocation, VkImage image)
{
  vmaDestroyImage(mData->allocator.get(), image, allocation);
}

auto VulkanAllocator::get_instance() -> VkInstance
{
  return mData->instance;
}

auto VulkanAllocator::get_device() -> VkDevice
{
  return mData->device;
}

auto VulkanAllocator::get_gpu() -> VkPhysicalDevice
{
  return mData->gpu;
}

}  // namespace glow
