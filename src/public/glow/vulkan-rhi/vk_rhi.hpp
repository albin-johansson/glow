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

#include "glow/core/prelude.hpp"
#include "glow/core/rhi/rhi.hpp"

#ifdef GLOW_BUILDING_VULKAN_RHI
  #define GLOW_VULKAN_API GLOW_EXPORT
#else
  #define GLOW_VULKAN_API GLOW_IMPORT
#endif  // GLOW_BUILDING_VULKAN_RHI

extern "C" {
GLOW_VULKAN_API void glow_init_rhi(glow::IRenderContext* context);
}  // extern "C"

struct VmaAllocationCreateInfo;
struct VmaAllocation_T;

using VmaAllocation = VmaAllocation_T*;

namespace glow {

class VulkanAllocator;
class VulkanBuffer;
class VulkanGraphicsPipeline;
class VulkanImage;
class VulkanRenderDevice;
class VulkanResourceManager;
class VulkanShaderModule;

}  // namespace glow