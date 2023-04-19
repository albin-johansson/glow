#include "renderer.hpp"

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>

// This comment prevents moving the following include before <imgui.h>
#include <ImGuizmo.h>

#include "common/debug/assert.hpp"
#include "common/predef.hpp"
#include "graphics/opengl/model.hpp"
#include "graphics/opengl/texture_2d.hpp"
#include "graphics/opengl/util.hpp"
#include "init/window.hpp"

namespace glow::gl {
namespace {

void _compile_and_link_program(Program& program,
                               const char* vert_path,
                               const char* frag_path)
{
  program.load_shader_files(vert_path, frag_path).check("Compilation error");
  program.link().check("Link error");
}

}  // namespace

Renderer::Renderer(SDL_Window* window)
    : mWindow {window}
{
  init_uniform_buffers();
  load_environment_program();
  load_shading_program();
  load_framebuffer_program();
}

void Renderer::init_uniform_buffers()
{
  mEnvUBO.bind();
  mEnvUBO.reserve_space(sizeof(EnvironmentBuffer));

  mMatrixUBO.bind();
  mMatrixUBO.reserve_space(sizeof(MatrixBuffer));

  mMaterialUBO.bind();
  mMaterialUBO.reserve_space(sizeof(MaterialBuffer));

  mFramebufferProgramOptionsUBO.bind();
  mFramebufferProgramOptionsUBO.reserve_space(sizeof(FramebufferProgramOptions));

  UniformBuffer::unbind();
}

void Renderer::load_environment_program()
{
  _compile_and_link_program(mEnvProgram,
                            "assets/shaders/gl/environment.vert",
                            "assets/shaders/gl/environment.frag");

  mEnvProgram.set_uniform("uEnvTexture", 0).check("uEnvTexture");
  mEnvProgram.set_uniform_block_binding("EnvironmentBuffer", 0).check("EnvBuffer UBO");
}

void Renderer::load_shading_program()
{
  _compile_and_link_program(mShadingProgram,
                            "assets/shaders/gl/shading.vert",
                            "assets/shaders/gl/shading.frag");

  mShadingProgram.set_uniform("uMaterialDiffuseTex", 5).check("uMaterialDiffuseTex");
  mShadingProgram.set_uniform_block_binding("MatrixBuffer", 0).check("Matrix UBO");
  mShadingProgram.set_uniform_block_binding("MaterialBuffer", 1).check("Material UBO");
}

void Renderer::load_framebuffer_program()
{
  _compile_and_link_program(mFramebufferProgram,
                            "assets/shaders/gl/framebuffer.vert",
                            "assets/shaders/gl/framebuffer.frag");

  mFramebufferProgram.set_uniform("uFramebuffer", 0).check("uFramebuffer");
  mFramebufferProgram.set_uniform_block_binding("Options", 0)
      .check("FramebufferProgramOptions");
}

void Renderer::begin_frame()
{
  mFrameStart = Clock::now();

  ImGui_ImplSDL2_NewFrame();
  ImGui_ImplOpenGL3_NewFrame();
  ImGui::NewFrame();
  ImGuizmo::BeginFrame();
}

void Renderer::end_frame()
{
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  // Measure the render pass before swapping the framebuffers, avoiding VSync idle time.
  const auto render_pass_end = Clock::now();
  mFrameDuration = chrono::duration_cast<Microseconds>(render_pass_end - mFrameStart);
}

void Renderer::swap_buffers()
{
  if constexpr (kIsMacOS) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  }

  SDL_GL_SwapWindow(mWindow);
}

void Renderer::bind_shading_program()
{
  mMatrixUBO.bind_block(0);
  mMaterialUBO.bind_block(1);
  mShadingProgram.bind();
}

void Renderer::unbind_shading_program()
{
  GLOW_ASSERT(get_bound_program() == mShadingProgram.get_id());

  VertexArray::unbind();
  Program::unbind();
  UniformBuffer::unbind_block(0);
  UniformBuffer::unbind_block(1);
}

void Renderer::render_buffer_to_screen(const Framebuffer& framebuffer)
{
  Framebuffer::unbind();

  Vec2i viewport_size {};
  SDL_GetWindowSizeInPixels(get_window(), &viewport_size.x, &viewport_size.y);

  glViewport(0, 0, viewport_size.x, viewport_size.y);

  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, framebuffer.get_color_texture_id());

  mFramebufferProgramOptionsUBO.bind();
  mFramebufferProgramOptionsUBO.update_data(0,
                                            sizeof mFramebufferProgramOptions,
                                            &mFramebufferProgramOptions);
  UniformBuffer::unbind();

  mFramebufferProgramOptionsUBO.bind_block(0);
  mFramebufferProgram.bind();

  mQuad.draw_without_depth_test();
}

void Renderer::render_environment(const Texture2D& texture)
{
  glActiveTexture(GL_TEXTURE0);
  texture.bind();

  mEnvUBO.bind();
  mEnvUBO.update_data(0, sizeof mEnvBuffer, &mEnvBuffer);
  UniformBuffer::unbind();

  mEnvUBO.bind_block(0);
  mEnvProgram.bind();

  mQuad.draw_without_depth_test();

  Program::unbind();
  Texture2D::unbind();
  UniformBuffer::unbind_block(0);
}

void Renderer::render_shaded_mesh(const Mesh& mesh, const Material& material)
{
  GLOW_ASSERT(get_bound_program() == mShadingProgram.get_id());

  mMatrixUBO.bind();
  mMatrixUBO.update_data(0, sizeof mMatrixBuffer, &mMatrixBuffer);

  mMaterialUBO.bind();
  mMaterialUBO.update_data(0, sizeof mMaterialBuffer, &mMaterialBuffer);

  if (mMaterialBuffer.has_diffuse_tex) {
    glActiveTexture(GL_TEXTURE5);
    Texture2D::bind(material.diffuse_tex.value());
  }

  mesh.vao.bind();
  glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, nullptr);
}

}  // namespace glow::gl
