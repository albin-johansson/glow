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
#include "graphics/opengl/framebuffer.hpp"
#include "graphics/opengl/program.hpp"
#include "graphics/opengl/renderer.hpp"
#include "graphics/opengl/texture_2d.hpp"
#include "graphics/opengl/uniform_buffer.hpp"
#include "ui/gizmos.hpp"

namespace glow {

/// Implements an OpenGL 4.1.0 renderer backend.
class OpenGLBackend final : public Backend {
 public:
  explicit OpenGLBackend(SDL_Window* window);

  void stop() override;

  void on_init(Scene& scene) override;

  void on_event(const SDL_Event& event) override;

  auto begin_frame(const Scene& scene) -> Result override;

  void end_frame() override;

  void render_scene(const Scene& scene, Dispatcher& dispatcher) override;

  void set_environment_texture(Scene& scene, const Path& path) override;

  void load_model(Scene& scene, const Path& path) override;

  [[nodiscard]] auto should_quit() const -> bool override { return mQuit; }

 private:
  gl::Renderer mRenderer;
  Maybe<gl::Texture2D> mEnvTexture;
  gl::Framebuffer mOffscreenFB;
  bool mQuit {false};

  void render_environment(const Scene& scene,
                          const Mat4& projection,
                          const Mat4& view,
                          Entity camera_entity);

  void render_models(const Scene& scene,
                     const Mat4& projection,
                     const Mat4&,
                     Dispatcher& dispatcher);
};

}  // namespace glow
