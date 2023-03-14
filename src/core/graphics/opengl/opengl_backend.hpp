#pragma once

#include <SDL2/SDL.h>

#include "common/predef.hpp"
#include "common/type/chrono.hpp"
#include "common/type/ecs.hpp"
#include "common/type/math.hpp"
#include "common/type/maybe.hpp"
#include "common/type/path.hpp"
#include "graphics/opengl/framebuffer.hpp"
#include "graphics/opengl/program.hpp"
#include "graphics/opengl/quad.hpp"
#include "graphics/opengl/texture_2d.hpp"
#include "graphics/opengl/uniform_buffer.hpp"
#include "graphics/perspective_camera.hpp"

namespace gravel {
GRAVEL_FORWARD_DECLARE(Scene);
}  // namespace gravel

namespace gravel::gl {

/// This struct corresponds to a std140 layout uniform block.
struct EnvironmentBuffer final {
  Mat4 inverse_proj_view {};
  Vec4 camera_pos {};
  float32 brightness {1.0f};
  float32 gamma {2.2f};
  int32 use_gamma_correction {true};
};

/// This struct corresponds to a std140 layout uniform block.
struct MatrixBuffer final {
  Mat4 m {};       ///< Model matrix.
  Mat4 mv {};      ///< Model-view matrix.
  Mat4 mvp {};     ///< Model-view-projection matrix.
  Mat4 normal {};  ///< Normal matrix.
};

/// This struct corresponds to a std140 layout uniform block.
struct MaterialBuffer final {
  Vec4 ambient {};
  Vec4 diffuse {};
  Vec4 specular {};
  Vec4 emission {};
  int32 has_ambient_tex {false};
  int32 has_diffuse_tex {false};
  int32 has_specular_tex {false};
  int32 has_emission_tex {false};
};

class OpenGLBackend final {
 public:
  explicit OpenGLBackend(SDL_Window* window);

  void update(float dt);

  void render(Scene& scene);

  void load_environment_texture(const Path& path);

  [[nodiscard]] auto should_quit() const -> bool { return mQuit; }

 private:
  SDL_Window* mWindow {};

  PerspectiveCamera mCamera;
  Quad mFullscreenQuad;

  Maybe<Texture2D> mEnvTexture;

  Program mFramebufferProgram;
  Program mEnvProgram;
  Program mBasicProgram;

  UniformBuffer mEnvProgramUbo;
  UniformBuffer mDynamicMatricesUbo;
  UniformBuffer mMaterialUbo;

  Framebuffer mPrimaryBuffer;

  float mCameraSpeed {10};
  float mCameraSensitivity {1};
  Vec2 mLastMousePos {};

  EnvironmentBuffer mEnvBuffer;
  MatrixBuffer mMatrixBuffer;
  MaterialBuffer mMaterialBuffer;

  Microseconds mRenderPassDuration {};

  // Debug options
  bool mDepthTest {true};
  bool mCullFaces {true};
  bool mBlending {true};
  bool mWireframe {false};
  bool mVSync {true};
  bool mHideUI {false};
  bool mQuit {false};

  void load_framebuffer_program();
  void load_environment_program();
  void load_basic_program();

  void init_uniform_buffers();

  void update_camera_position(float dt);
  void update_camera_direction(float dt);

  void render_environment(const Mat4& projection, const Mat4& view);

  void render_models(Scene& scene, const Mat4& projection, const Mat4& view);

  void render_buffer_to_screen(const Framebuffer& buffer);

  void render_gui(Scene& scene);
  void render_node_gui(Scene& scene, Entity entity);
};

}  // namespace gravel::gl
