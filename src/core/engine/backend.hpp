#pragma once

#include <SDL2/SDL.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/result.hpp"
#include "common/type/dispatcher.hpp"
#include "common/type/math.hpp"
#include "common/type/memory.hpp"
#include "common/type/path.hpp"
#include "graphics/graphics_api.hpp"

namespace glow {

GRAVEL_FORWARD_DECLARE_C(Scene);

class Backend {
 public:
  virtual ~Backend() = default;

  virtual void stop() = 0;

  virtual void on_init([[maybe_unused]] Scene& scene) {}
  virtual void on_quit() {}

  virtual void on_event(const SDL_Event& event) = 0;

  virtual auto begin_frame(const Scene& scene) -> Result = 0;

  virtual void end_frame() = 0;

  virtual void render_scene(const Scene& scene,
                            const Vec2& framebuffer_size,
                            Dispatcher& dispatcher) = 0;

  virtual void set_environment_texture(Scene& scene, const Path& path) = 0;

  virtual void load_model(Scene& scene, const Path& path) = 0;

  [[nodiscard]] virtual auto should_quit() const -> bool = 0;

  [[nodiscard]] virtual auto get_primary_framebuffer_handle() -> void* = 0;
};

[[nodiscard]] auto create_backend(SDL_Window* window, GraphicsApi api) -> Unique<Backend>;

}  // namespace glow
