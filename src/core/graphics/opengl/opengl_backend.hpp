#pragma once

#include <SDL2/SDL.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/chrono.hpp"
#include "common/type/ecs.hpp"
#include "common/type/math.hpp"
#include "common/type/maybe.hpp"
#include "common/type/path.hpp"
#include "engine/backend.hpp"
#include "graphics/opengl/buffers.hpp"
#include "graphics/opengl/framebuffer.hpp"
#include "graphics/opengl/program.hpp"
#include "graphics/opengl/renderer.hpp"
#include "graphics/opengl/texture_2d.hpp"
#include "graphics/opengl/uniform_buffer.hpp"
#include "ui/gizmos.hpp"

namespace gravel::gl {

class OpenGLBackend final : public Backend {
 public:
  explicit OpenGLBackend(SDL_Window* window);

  void stop() override;

  void on_init(Scene& scene) override;

  void on_event(const SDL_Event& event) override;

  void begin_frame() override;

  void end_frame() override;

  void render_scene(const Scene& scene,
                    const Vec2& framebuffer_size,
                    Dispatcher& dispatcher) override;

  void load_environment_texture(const Path& path);

  [[nodiscard]] auto get_primary_framebuffer_handle() -> void* override;

  [[nodiscard]] auto should_quit() const -> bool override { return mQuit; }

 private:
  Renderer mRenderer;
  Maybe<Texture2D> mEnvTexture;
  Framebuffer mPrimaryFBO;
  bool mQuit {false};

  void make_main_camera_node(Scene& scene);

  void render_environment(const Scene& scene,
                          const Mat4& projection,
                          const Mat4& view,
                          Entity camera_entity);

  void render_models(const Scene& scene,
                     const Mat4& projection,
                     const Mat4&,
                     Dispatcher& dispatcher);
};

}  // namespace gravel::gl
