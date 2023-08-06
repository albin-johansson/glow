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

#pragma once

#include <memory>  // unique_ptr

#include <vulkan/vulkan.h>

#include "glow/core/prelude.hpp"
#include "glow/vulkan-rhi/vk_rhi.hpp"

namespace glow {

class GLOW_VULKAN_API VulkanAllocator final {
 public:
  GLOW_CANNOT_COPY(VulkanAllocator);
  GLOW_DEFAULT_MOVE(VulkanAllocator);

  VulkanAllocator(VkInstance instance, VkDevice device, VkPhysicalDevice gpu);

  ~VulkanAllocator() noexcept;

  [[nodiscard]] auto make_buffer(const VmaAllocationCreateInfo& allocation_info,
                                 const VkBufferCreateInfo& buffer_info,
                                 VmaAllocation* out_allocation,
                                 VkBuffer* out_buffer) -> VkResult;

  [[nodiscard]] auto make_image(const VmaAllocationCreateInfo& allocation_info,
                                const VkImageCreateInfo& image_info,
                                VmaAllocation* out_allocation,
                                VkImage* out_image) -> VkResult;

  void destroy_buffer(VmaAllocation allocation, VkBuffer buffer);

  void destroy_image(VmaAllocation allocation, VkImage image);

  [[nodiscard]] auto get_instance() -> VkInstance;

  [[nodiscard]] auto get_device() -> VkDevice;

  [[nodiscard]] auto get_gpu() -> VkPhysicalDevice;

 private:
  struct Data;
  std::unique_ptr<Data> mData;
};

}  // namespace glow
