#pragma once

#include <SDL2/SDL.h>

#include "common/type/math.hpp"
#include "common/type/path.hpp"
#include "graphics/opengl/framebuffer.hpp"
#include "graphics/opengl/program.hpp"
#include "graphics/opengl/quad.hpp"
#include "graphics/opengl/texture_2d.hpp"
#include "graphics/opengl/uniform_buffer.hpp"
#include "graphics/perspective_camera.hpp"

namespace gravel::gl {

/// Beware, this struct corresponds to a std140 layout uniform block.
struct EnvironmentOptions final {
  Mat4 inverse_proj_view {};
  Vec4 camera_pos {};
  float32 brightness {1.0f};
  float32 gamma {2.2f};
  int32 use_gamma_correction {true};
};

class OpenGLBackend final {
 public:
  explicit OpenGLBackend(SDL_Window* window);

  void update(float dt);

  void render();

  [[nodiscard]] auto should_quit() const -> bool { return mQuit; }

 private:
  SDL_Window* mWindow {};

  PerspectiveCamera mCamera;
  Quad mFullscreenQuad;

  Texture2D mEnvTexture;
  Texture2D mExplosionTexture;

  Program mFramebufferProgram;
  Program mEnvProgram;

  UniformBuffer mEnvProgramUbo;

  Framebuffer mPrimaryBuffer;

  float mCameraSpeed {1};
  float mCameraSensitivity {1};
  Vec2 mLastMousePos {};

  EnvironmentOptions mEnvOptions;

  // Debug options
  bool mDepthTest {true};
  bool mCullFaces {true};
  bool mBlending {true};
  bool mWireframe {false};
  bool mQuit {false};

  void load_framebuffer_program();
  void load_environment_program();

  void load_environment_texture(const Path& path);

  void update_camera_position(float dt);
  void update_camera_direction(float dt);

  void render_environment(const Mat4& projection, const Mat4& view);

  void render_buffer_to_screen(const Framebuffer& buffer);

  void render_gui();
};

}  // namespace gravel::gl
