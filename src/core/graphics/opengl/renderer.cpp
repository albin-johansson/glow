#include "renderer.hpp"

#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>

// This comment prevents moving the following include before <imgui.h>
#include <ImGuizmo.h>

#include "common/debug/assert.hpp"
#include "common/predef.hpp"
#include "graphics/opengl/model.hpp"
#include "graphics/opengl/util.hpp"
#include "texture_2d.hpp"

namespace gravel::gl {
namespace {

void compile_and_link_program(Program& program,
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
}

void Renderer::init_uniform_buffers()
{
  mEnvUBO.bind();
  mEnvUBO.reserve_space(sizeof(EnvironmentBuffer));

  mMatrixUBO.bind();
  mMatrixUBO.reserve_space(sizeof(MatrixBuffer));

  mMaterialUBO.bind();
  mMaterialUBO.reserve_space(sizeof(MaterialBuffer));

  UniformBuffer::unbind();
}

void Renderer::load_environment_program()
{
  compile_and_link_program(mEnvProgram,
                           "assets/shaders/environment.vert",
                           "assets/shaders/environment.frag");

  mEnvProgram.set_uniform("env_texture", 0).check("env_texture");
  mEnvProgram.set_uniform_block_binding("EnvironmentBuffer", 0)
      .check("EnvironmentBuffer UBO");
}

void Renderer::load_shading_program()
{
  compile_and_link_program(mShadingProgram,
                           "assets/shaders/shading.vert",
                           "assets/shaders/shading.frag");

  mShadingProgram.set_uniform("material_diffuse_tex", 5).check("material_diffuse_tex");
  mShadingProgram.set_uniform_block_binding("MatrixBuffer", 0).check("Matrix UBO");
  mShadingProgram.set_uniform_block_binding("MaterialBuffer", 1).check("Material UBO");
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
  if constexpr (kOnApple) {
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
  GRAVEL_ASSERT(get_bound_program() == mShadingProgram.get_id());

  VertexArray::unbind();
  Program::unbind();
  UniformBuffer::unbind_block(0);
  UniformBuffer::unbind_block(1);
}

void Renderer::render_environment(const Texture2D& texture,
                                  const Mat4& projection,
                                  const Mat4& view,
                                  const Vec3& camera_position)
{
  glActiveTexture(GL_TEXTURE0);
  texture.bind();

  mEnvBuffer.inverse_proj_view = glm::inverse(projection * view);
  mEnvBuffer.camera_pos = Vec4 {camera_position, 0};

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

void Renderer::render_shaded_mesh(const Mesh& mesh,
                                  const Material& material,
                                  const Mat4& model,
                                  const Mat4& view,
                                  const Mat4& projection)
{
  GRAVEL_ASSERT(get_bound_program() == mShadingProgram.get_id());

  mMatrixBuffer.update(model, view, projection);
  mMaterialBuffer.update(material);

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

}  // namespace gravel::gl
