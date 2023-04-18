#pragma once

#include <array>  // size

#include <vulkan/vulkan.h>

#include "common/primitives.hpp"
#include "graphics/vulkan/pipeline/descriptor_pool.hpp"
#include "graphics/vulkan/pipeline/pipeline_cache.hpp"

namespace glow::vk {

inline constexpr VkDescriptorPoolSize kImGuiDescriptorPoolSizes[] = {
    {VK_DESCRIPTOR_TYPE_SAMPLER, 8},
    {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 32},
    {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 8},
    {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 8},
    {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 8},
    {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 8},
    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 32},
    {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 8},
    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 8},
    {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 8},
    {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 8},
};

struct ImGuiData final {
  PipelineCachePtr pipeline_cache {create_pipeline_cache()};
  DescriptorPool descriptor_pool {1'000,
                                  kImGuiDescriptorPoolSizes,
                                  std::size(kImGuiDescriptorPoolSizes),
                                  VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT};
};

/// Initializes the ImGui Vulkan backend.
void init_imgui(ImGuiData& data, VkRenderPass render_pass, uint32 swapchain_images);

}  // namespace glow::vk
