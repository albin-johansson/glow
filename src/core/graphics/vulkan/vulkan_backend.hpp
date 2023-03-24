#pragma once

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "engine/backend.hpp"

namespace gravel::vk {

class VulkanBackend final : public Backend {
 public:
  explicit VulkanBackend(SDL_Window* window);

  void stop() override;

  void on_init(Scene& scene) override;

  void on_event(const SDL_Event& event) override;

  void begin_frame() override;

  void end_frame() override;

  void render_scene(const Scene& scene,
                    const Vec2& framebuffer_size,
                    Dispatcher& dispatcher) override;

  [[nodiscard]] auto get_primary_framebuffer_handle() -> void* override;

  [[nodiscard]] auto should_quit() const -> bool override { return mQuit; }

  [[nodiscard]] auto get_name() const -> const char* override { return "Vulkan 1.2"; }

 private:
  bool mQuit {false};
};

}  // namespace gravel::vk
