#include "dear_imgui_vulkan.hpp"

#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

#include "common/debug/error.hpp"
#include "graphics/vulkan/command_buffer.hpp"
#include "graphics/vulkan/context.hpp"

namespace glow {

DearImGuiVulkan::DearImGuiVulkan(SDL_Window* window)
{
  if (!ImGui_ImplSDL2_InitForVulkan(window)) {
    throw Error {"[GUI] Could not initialize DearImGui SDL2 backend"};
  }
}

DearImGuiVulkan::~DearImGuiVulkan()
{
  ImGui_ImplSDL2_Shutdown();
}

void DearImGuiVulkan::recreate_font_textures()
{
  vk::execute_immediately(vk::get_graphics_command_pool(),
                          [](VkCommandBuffer cmd_buffer) {
                            ImGui_ImplVulkan_CreateFontsTexture(cmd_buffer);
                          });

  ImGui_ImplVulkan_DestroyFontUploadObjects();
}

}  // namespace glow