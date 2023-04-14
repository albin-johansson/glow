#include "imgui.hpp"

#include <imgui_impl_vulkan.h>

#include "common/debug/error.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/physical_device.hpp"
#include "graphics/vulkan/util/constants.hpp"
#include "graphics/vulkan/util/vk_call.hpp"
#include "init/dear_imgui_vulkan.hpp"

namespace gravel::vk {

void init_imgui(ImGuiData& data, VkRenderPass render_pass, const uint32 swapchain_images)
{
  const auto graphics_queue_family_index =
      get_queue_family_indices(get_gpu(), get_surface()).graphics_family.value();

  ImGui_ImplVulkan_InitInfo info {};
  info.Instance = get_instance();
  info.PhysicalDevice = get_gpu();
  info.Device = get_device();
  info.QueueFamily = graphics_queue_family_index;
  info.Queue = get_graphics_queue();
  info.PipelineCache = data.pipeline_cache.get();
  info.DescriptorPool = data.descriptor_pool.get();
  info.Subpass = 0;
  info.MinImageCount = kMaxFramesInFlight;
  info.ImageCount = swapchain_images;
  info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  info.Allocator = nullptr;
  info.CheckVkResultFn = [](const VkResult result) {
    GRAVEL_VK_CALL(result, "[VK] ImGui Vulkan backend error");
  };

  if (!ImGui_ImplVulkan_Init(&info, render_pass)) {
    throw Error {"[VK] Could not initialize ImGui Vulkan backend"};
  }

  DearImGuiVulkan::recreate_font_textures();
}

}  // namespace gravel::vk
