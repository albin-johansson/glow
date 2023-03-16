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
#include "graphics/opengl/quad.hpp"
#include "graphics/opengl/texture_2d.hpp"
#include "graphics/opengl/uniform_buffer.hpp"
#include "graphics/perspective_camera.hpp"
#include "ui/gizmos.hpp"

namespace gravel {
GRAVEL_FORWARD_DECLARE_C(Scene);
}  // namespace gravel

namespace gravel::gl {

class OpenGLBackend final : public Backend {
 public:
  explicit OpenGLBackend(SDL_Window* window);

  void on_init(Scene& scene) override;

  void on_update(float32 dt) override;

  void on_render(Scene& scene) override;

  void load_environment_texture(const Path& path);

  [[nodiscard]] auto should_quit() const -> bool override { return mQuit; }

 private:
  SDL_Window* mWindow {};

  PerspectiveCamera mCamera;
  Quad mFullscreenQuad;

  Maybe<Texture2D> mEnvTexture;

  Program mFramebufferProgram;
  Program mEnvProgram;
  Program mBasicProgram;

  UniformBuffer mEnvironmentUBO;
  UniformBuffer mMatrixUBO;
  UniformBuffer mMaterialUBO;

  Framebuffer mPrimaryFBO;

  EnvironmentBuffer mEnvBuffer;
  MatrixBuffer mMatrixBuffer;
  MaterialBuffer mMaterialBuffer;

  float mCameraSpeed {5};
  float mCameraSensitivity {0.5f};
  Vec2i mViewportSize {};
  Vec2i mViewportResolution {};
  Microseconds mRenderPassDuration {};

  // Debug options
  bool mUseDepthTest {true};
  bool mUseFaceCulling {true};
  bool mUseBlending {true};
  bool mUseWireframe {false};
  bool mUseVSync {true};

  bool mRestoreLayout {true};
  bool mQuit {false};

  GizmoMode mGizmoMode {GizmoMode::Translate};

  void load_framebuffer_program();
  void load_environment_program();
  void load_basic_program();

  void init_uniform_buffers();

  void update_camera_position(float32 dt);

  void render_environment(const Mat4& projection, const Mat4& view);
  void render_models(Scene& scene, const Mat4& projection, const Mat4& view);
  void render_scene_viewport(Scene& scene);
  void render_dock_widgets(Scene& scene);
  void render_node_gui(Scene& scene, Entity entity);
};

}  // namespace gravel::gl
