#include "vulkan_backend.hpp"

#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

namespace gravel::vlk {

VulkanBackend::VulkanBackend(SDL_Window* window)
{
}

void VulkanBackend::stop()
{
  mQuit = true;
}

void VulkanBackend::on_init(Scene& scene)
{
}

void VulkanBackend::prepare_imgui_for_vulkan()
{
}

void VulkanBackend::on_event(const SDL_Event& event)
{
  ImGui_ImplSDL2_ProcessEvent(&event);

  if (event.type == SDL_QUIT) {
    mQuit = true;
  }
}

void VulkanBackend::begin_frame() {}

void VulkanBackend::end_frame() {}

void VulkanBackend::render_scene(const Scene& scene,
                                 const Vec2& framebuffer_size,
                                 Dispatcher& dispatcher)
{
}

auto VulkanBackend::get_primary_framebuffer_handle() -> void*
{
  return nullptr;
}

}  // namespace gravel::vlk
