#pragma once

#include <SDL2/SDL.h>

#include "common/predef.hpp"
#include "common/type/chrono.hpp"
#include "common/type/math.hpp"
#include "graphics/opengl/buffers.hpp"
#include "graphics/opengl/program.hpp"
#include "graphics/opengl/quad.hpp"
#include "graphics/opengl/uniform_buffer.hpp"

namespace gravel::gl {

GRAVEL_FORWARD_DECLARE_C(Texture2D);
GRAVEL_FORWARD_DECLARE_S(Mesh);
GRAVEL_FORWARD_DECLARE_S(Material);

class Renderer final {
 public:
  explicit Renderer(SDL_Window* window);

  void begin_frame();

  void end_frame();

  void swap_buffers();

  void bind_shading_program();
  void unbind_shading_program();

  void render_environment(const Texture2D& texture,
                          const Mat4& projection,
                          const Mat4& view,
                          const Vec3& camera_position);

  void render_shaded_mesh(const Mesh& mesh,
                          const Material& material,
                          const Mat4& model,
                          const Mat4& view,
                          const Mat4& projection);

  [[nodiscard]] auto get_frame_duration() const -> Duration { return mFrameDuration; }

 private:
  SDL_Window* mWindow {};
  Quad mQuad;

  // Shader programs
  Program mEnvProgram;
  Program mShadingProgram;

  // UBOs
  UniformBuffer mMatrixUBO;
  UniformBuffer mMaterialUBO;
  UniformBuffer mEnvUBO;

  // std140 layout buffer structs
  MatrixBuffer mMatrixBuffer;
  MaterialBuffer mMaterialBuffer;
  EnvironmentBuffer mEnvBuffer;

  // Performance info
  TimePoint mFrameStart {};
  Duration mFrameDuration {};

  void init_uniform_buffers();
  void load_environment_program();
  void load_shading_program();
};

}  // namespace gravel::gl
