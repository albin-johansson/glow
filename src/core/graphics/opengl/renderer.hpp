#pragma once

#include <SDL2/SDL.h>

#include "common/predef.hpp"
#include "common/type/chrono.hpp"
#include "common/type/math.hpp"
#include "graphics/opengl/framebuffer.hpp"
#include "graphics/opengl/program.hpp"
#include "graphics/opengl/quad.hpp"
#include "graphics/opengl/shader_buffers.hpp"
#include "graphics/opengl/uniform_buffer.hpp"

namespace glow::gl {

GLOW_FORWARD_DECLARE_C(Texture2D);
GLOW_FORWARD_DECLARE_S(Mesh);
GLOW_FORWARD_DECLARE_S(Material);

class Renderer final {
 public:
  explicit Renderer(SDL_Window* window);

  void begin_frame();

  void end_frame();

  void swap_buffers();

  void bind_shading_program();
  void unbind_shading_program();

  void render_buffer_to_screen(const Framebuffer& framebuffer);

  void render_environment(const Texture2D& texture);

  void render_shaded_mesh(const Mesh& mesh, const Material& material);

  [[nodiscard]] auto get_env_buffer() -> EnvironmentBuffer& { return mEnvBuffer; }

  [[nodiscard]] auto get_matrix_buffer() -> MatrixBuffer& { return mMatrixBuffer; }

  [[nodiscard]] auto get_material_buffer() -> MaterialBuffer& { return mMaterialBuffer; }

  [[nodiscard]] auto get_framebuffer_program_options_buffer()
      -> FramebufferProgramOptions&
  {
    return mFramebufferProgramOptions;
  }

  [[nodiscard]] auto get_frame_duration() const -> Duration { return mFrameDuration; }

 private:
  SDL_Window* mWindow {};
  Quad mQuad;

  // Shader programs
  Program mEnvProgram;
  Program mShadingProgram;
  Program mFramebufferProgram;

  // UBOs
  UniformBuffer mMatrixUBO;
  UniformBuffer mMaterialUBO;
  UniformBuffer mEnvUBO;
  UniformBuffer mFramebufferProgramOptionsUBO;

  // std140 layout buffer structs
  MatrixBuffer mMatrixBuffer;
  MaterialBuffer mMaterialBuffer;
  EnvironmentBuffer mEnvBuffer;
  FramebufferProgramOptions mFramebufferProgramOptions;

  // Performance info
  TimePoint mFrameStart {};
  Duration mFrameDuration {};

  void init_uniform_buffers();
  void load_environment_program();
  void load_shading_program();
  void load_framebuffer_program();
};

}  // namespace glow::gl
