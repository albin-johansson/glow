#include "opengl_backend.hpp"

#include <cstdlib>  // offsetof

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>
#include <imgui_internal.h>
#include <spdlog/spdlog.h>

// This comment prevents moving the following include before <imgui.h>
#include <ImGuizmo.h>

#include "common/debug/assert.hpp"
#include "common/predef.hpp"
#include "graphics/opengl/model.hpp"
#include "graphics/opengl/util.hpp"
#include "io/texture_loader.hpp"
#include "scene/identifier.hpp"
#include "scene/node.hpp"
#include "scene/scene.hpp"
#include "scene/transform.hpp"
#include "util/bits.hpp"

namespace gravel::gl {

OpenGLBackend::OpenGLBackend(SDL_Window* window)
    : mWindow {window}
{
  load_framebuffer_program();
  load_environment_program();
  load_basic_program();
  init_uniform_buffers();

  mCamera.set_position(Vec3 {0, 0, 2});
}

void OpenGLBackend::load_framebuffer_program()
{
  const char* vert_path = "assets/shaders/framebuffer.vert";
  const char* frag_path = "assets/shaders/framebuffer.frag";

  mFramebufferProgram.load_shader_files(vert_path, frag_path).check("Compilation error");
  mFramebufferProgram.link().check("Link error");

  mFramebufferProgram.set_uniform("framebuffer", 0).check("framebuffer");
}

void OpenGLBackend::load_environment_program()
{
  const char* vert_path = "assets/shaders/environment.vert";
  const char* frag_path = "assets/shaders/environment.frag";

  mEnvProgram.load_shader_files(vert_path, frag_path).check("Compilation error");
  mEnvProgram.link().check("Link error");

  mEnvProgram.set_uniform("env_texture", 0).check("env_texture");

  mEnvProgram.set_uniform_block_binding("EnvironmentBuffer", 0)
      .check("EnvironmentBuffer UBO");
}

void OpenGLBackend::load_basic_program()
{
  const char* vert_path = "assets/shaders/basic.vert";
  const char* frag_path = "assets/shaders/basic.frag";

  mBasicProgram.load_shader_files(vert_path, frag_path).check("Compilation error");
  mBasicProgram.link().check("Link error");

  mBasicProgram.set_uniform("material_diffuse_tex", 5).check("material_diffuse_tex");

  mBasicProgram.set_uniform_block_binding("MatrixBuffer", 0).check("Matrix UBO");
  mBasicProgram.set_uniform_block_binding("MaterialBuffer", 1).check("Material UBO");
}

void OpenGLBackend::init_uniform_buffers()
{
  mEnvironmentUBO.bind();
  mEnvironmentUBO.reserve_space(sizeof(EnvironmentBuffer));

  mMatrixUBO.bind();
  mMatrixUBO.reserve_space(sizeof(MatrixBuffer));

  mMaterialUBO.bind();
  mMaterialUBO.reserve_space(sizeof(MaterialBuffer));

  UniformBuffer::unbind();
}

void OpenGLBackend::load_environment_texture(const Path& path)
{
  mEnvTexture = Texture2D::load_rgb_f32(path);
}

void OpenGLBackend::update(const float32 dt)
{
  update_camera_position(dt);

  SDL_Event event {};
  while (SDL_PollEvent(&event)) {
    ImGui_ImplSDL2_ProcessEvent(&event);

    if (event.type == SDL_QUIT) {
      mQuit = true;
    }
  }
}

void OpenGLBackend::update_camera_position(const float32 dt)
{
  const uint8* state = SDL_GetKeyboardState(nullptr);

  const auto& direction = mCamera.get_direction();
  const auto& up = mCamera.get_world_up();

  if (state[SDL_SCANCODE_W]) {
    mCamera.offset_position(direction * mCameraSpeed * dt);
  }
  else if (state[SDL_SCANCODE_S]) {
    mCamera.offset_position(-direction * mCameraSpeed * dt);
  }

  if (state[SDL_SCANCODE_A]) {
    const auto cross = glm::cross(direction, up);
    mCamera.offset_position(-cross * mCameraSpeed * dt);
  }
  else if (state[SDL_SCANCODE_D]) {
    const auto cross = glm::cross(direction, up);
    mCamera.offset_position(cross * mCameraSpeed * dt);
  }

  if (state[SDL_SCANCODE_E]) {
    mCamera.offset_position(up * mCameraSpeed * dt);
  }
  else if (state[SDL_SCANCODE_Q]) {
    mCamera.offset_position(-up * mCameraSpeed * dt);
  }
}

void OpenGLBackend::render(Scene& scene)
{
  const auto render_pass_start = Clock::now();

  ImGui_ImplSDL2_NewFrame();
  ImGui_ImplOpenGL3_NewFrame();
  ImGui::NewFrame();
  ImGuizmo::BeginFrame();

  const auto root_dock_id = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

  if (mRestoreLayout) {
    const auto size = ImGui::GetMainViewport()->Size;
    if (size.x > 0 && size.y > 0) {
      ImGui::DockBuilderRemoveNodeChildNodes(root_dock_id);

      auto id = root_dock_id;
      auto right = ImGui::DockBuilderSplitNode(id, ImGuiDir_Right, 0.33f, nullptr, &id);
      auto right_bottom =
          ImGui::DockBuilderSplitNode(right, ImGuiDir_Down, 0.5f, nullptr, &right);

      ImGui::DockBuilderDockWindow("Viewport", id);
      ImGui::DockBuilderDockWindow("Scene", right);
      ImGui::DockBuilderDockWindow("Information", right_bottom);

      ImGui::DockBuilderFinish(id);

      mRestoreLayout = false;
    }
  }

  // Reset options
  set_option(GL_DEPTH_TEST, mUseDepthTest);
  set_option(GL_CULL_FACE, mUseFaceCulling);
  set_option(GL_BLEND, mUseBlending);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  render_scene_viewport(scene);
  render_dock_widgets(scene);

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  GRAVEL_GL_CHECK_ERRORS();

  // Measure the render pass before swapping the framebuffers, avoiding VSync idle time.
  const auto render_pass_end = Clock::now();
  mRenderPassDuration =
      chrono::duration_cast<Microseconds>(render_pass_end - render_pass_start);

  if constexpr (kOnApple) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  }

  SDL_GL_SwapWindow(mWindow);
}

void OpenGLBackend::render_environment(const Mat4& projection, const Mat4& view)
{
  GRAVEL_ASSERT(get_bound_framebuffer() == mPrimaryFBO.get_id());

  if (!mEnvTexture.has_value()) {
    return;
  }

  glActiveTexture(GL_TEXTURE0);
  mEnvTexture->bind();

  mEnvBuffer.inverse_proj_view = glm::inverse(projection * view);
  mEnvBuffer.camera_pos = Vec4 {mCamera.get_position(), 0};

  mEnvironmentUBO.bind();
  mEnvironmentUBO.update_data(0, sizeof mEnvBuffer, &mEnvBuffer);
  UniformBuffer::unbind();

  mEnvironmentUBO.bind_block(0);
  mEnvProgram.bind();
  mFullscreenQuad.draw_without_depth_test();

  Program::unbind();
  Texture2D::unbind();
  UniformBuffer::unbind_block(0);
}

void OpenGLBackend::render_models(Scene& scene, const Mat4& projection, const Mat4& view)
{
  mMatrixUBO.bind_block(0);
  mMaterialUBO.bind_block(1);

  mBasicProgram.bind();

  auto& registry = scene.get_registry();
  for (auto [entity, transform, model] : registry.view<Transform, gl::Model>().each()) {
    const auto model_transform = transform.to_model_matrix();

    for (auto& mesh : model.meshes) {
      const auto& material = registry.get<gl::Material>(mesh.material);

      mMatrixBuffer.m = model_transform * mesh.transform;
      mMatrixBuffer.mv = view * mMatrixBuffer.m;
      mMatrixBuffer.mvp = projection * mMatrixBuffer.mv;
      mMatrixBuffer.normal = glm::inverse(glm::transpose(mMatrixBuffer.mv));

      mMatrixUBO.bind();
      mMatrixUBO.update_data(0, sizeof mMatrixBuffer, &mMatrixBuffer);

      mMaterialBuffer.ambient = Vec4 {material.ambient, 0};
      mMaterialBuffer.diffuse = Vec4 {material.diffuse, 0};
      mMaterialBuffer.specular = Vec4 {material.specular, 0};
      mMaterialBuffer.emission = Vec4 {material.emission, 0};

      mMaterialBuffer.has_diffuse_tex = material.diffuse_tex.has_value();
      mMaterialBuffer.has_specular_tex = material.specular_tex.has_value();

      mMaterialUBO.bind();
      mMaterialUBO.update_data(0, sizeof mMaterialBuffer, &mMaterialBuffer);

      if (mMaterialBuffer.has_diffuse_tex) {
        glActiveTexture(GL_TEXTURE5);
        gl::Texture2D::bind(material.diffuse_tex.value());
      }

      mesh.vao.bind();
      glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, nullptr);
    }

    VertexArray::unbind();
    UniformBuffer::unbind();

    ImGuizmo::SetID(static_cast<int>(entity));
    show_model_control_gizmo(mGizmoMode, projection, view, transform);
  }

  GRAVEL_GL_CHECK_ERRORS();

  Program::unbind();

  Texture2D::unbind();
  UniformBuffer::unbind_block(0);
  UniformBuffer::unbind_block(1);
}

void OpenGLBackend::render_scene_viewport(Scene& scene)
{
  ImGuiWindowClass wc {};
  wc.DockNodeFlagsOverrideSet =
      ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDockingOverMe;
  ImGui::SetNextWindowClass(&wc);

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});

  if (ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar)) {
    const auto window_pos = ImGui::GetWindowPos();
    const auto window_size = ImGui::GetWindowSize();
    ImGuizmo::SetRect(window_pos.x, window_pos.y, window_size.x, window_size.y);

    if (!ImGuizmo::IsUsing() && ImGui::IsItemActive() && ImGui::IsWindowHovered()) {
      const auto mouse_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left, 0.05f);

      const auto dt = ImGui::GetIO().DeltaTime;
      const auto yaw = mCameraSensitivity * -mouse_delta.x * dt;
      const auto pitch = mCameraSensitivity * -mouse_delta.y * dt;
      mCamera.rotate_direction(yaw, pitch);

      ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
    }

    const auto widget_size = ImGui::GetWindowSize();
    const auto framebuffer_size = widget_size * ImGui::GetIO().DisplayFramebufferScale;

    mViewportSize.x = static_cast<int>(widget_size.x);
    mViewportSize.y = static_cast<int>(widget_size.y);
    mViewportResolution.x = static_cast<int>(framebuffer_size.x);
    mViewportResolution.y = static_cast<int>(framebuffer_size.y);

    // Update framebuffer dimensions
    mPrimaryFBO.bind();
    mPrimaryFBO.resize(mViewportResolution);
    mPrimaryFBO.clear();

    mCamera.set_aspect_ratio(framebuffer_size.x / framebuffer_size.y);

    const auto projection = mCamera.to_projection_matrix();
    const auto view = mCamera.to_view_matrix();

    // Render the environment backdrop
    render_environment(projection, view);

    glPolygonMode(GL_FRONT_AND_BACK, mUseWireframe ? GL_LINE : GL_FILL);
    render_models(scene, projection, view);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    Framebuffer::unbind();

    ImGui::Image(bitcast<ImTextureID>(static_cast<uintptr>(mPrimaryFBO.get_id())),
                 ImGui::GetContentRegionAvail(),
                 ImVec2 {0, 1},
                 ImVec2 {1, 0});
  }
  ImGui::End();
  ImGui::PopStyleVar();
}

void OpenGLBackend::render_dock_widgets(Scene& scene)
{
  if (ImGui::Begin("Information", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    const auto& io = ImGui::GetIO();
    ImGui::SeparatorText("Performance");
    ImGui::Text("FPS: %.2f", io.Framerate);
    ImGui::Text("Render pass: %.3f ms",
                static_cast<float64>(mRenderPassDuration.count()) / 1'000.0);
    if (ImGui::Checkbox("VSync", &mUseVSync)) {
      SDL_GL_SetSwapInterval(mUseVSync ? 1 : 0);
    }
    ImGui::Checkbox("Face culling", &mUseFaceCulling);

    ImGui::SeparatorText("Debug");
    ImGui::Checkbox("Blending", &mUseBlending);
    ImGui::Checkbox("Wireframe", &mUseWireframe);
    ImGui::Checkbox("Depth test", &mUseDepthTest);

    const auto& cam_pos = mCamera.get_position();
    const auto& cam_dir = mCamera.get_direction();
    float fov = mCamera.get_fov();
    float near_plane = mCamera.get_near_plane();
    float far_plane = mCamera.get_far_plane();
    ImGui::SeparatorText("Camera");
    ImGui::Text("Position: (%.1f, %.1f, %.1f)", cam_pos.x, cam_pos.y, cam_pos.z);
    ImGui::Text("Direction: (%.2f, %.2f, %.2f)", cam_dir.x, cam_dir.y, cam_dir.z);
    ImGui::Text("Aspect ratio: %.2f", mCamera.get_aspect_ratio());
    ImGui::SliderFloat("FOV", &fov, 30, 120);
    ImGui::SliderFloat("Near plane", &near_plane, 0.01f, 1.0f);
    ImGui::SliderFloat("Far plane", &far_plane, near_plane, near_plane + 1'000'000);
    ImGui::SliderFloat("Speed",
                       &mCameraSpeed,
                       0.1f,
                       2000.0f,
                       "%.3f",
                       ImGuiSliderFlags_Logarithmic);
    ImGui::SliderFloat("Sensitivity", &mCameraSensitivity, 0.1f, 5.0f);
    mCamera.set_fov(fov);
    mCamera.set_near_plane(near_plane);
    mCamera.set_far_plane(far_plane);

    ImGui::SeparatorText("Viewport");
    ImGui::Text("Size: (%d, %d)", mViewportSize.x, mViewportSize.y);
    ImGui::Text("Resolution: (%d, %d)", mViewportResolution.x, mViewportResolution.y);

    ImGui::SeparatorText("Environment");

    bool use_gamma = mEnvBuffer.use_gamma_correction;
    ImGui::Checkbox("Gamma correction", &use_gamma);
    mEnvBuffer.use_gamma_correction = use_gamma;

    ImGui::SliderFloat("Gamma", &mEnvBuffer.gamma, 1.0f, 4.0f);
    ImGui::SliderFloat("Brightness", &mEnvBuffer.brightness, 0.0f, 1.0f, "%.2f");

    ImGui::SeparatorText("System");
    ImGui::TextUnformatted("API: OpenGL 4.1.0 core");

    ImGui::SeparatorText("Misc");
    if (ImGui::Button("Reset Layout")) {
      mRestoreLayout = true;
    }
  }
  ImGui::End();

  if (ImGui::Begin("Scene")) {
    const auto exclusions = entt::exclude<ChildNode>;
    const auto view = scene.get_registry().view<Node>(exclusions);
    for (auto [entity, node] : view.each()) {
      render_node_gui(scene, entity);
    }
  }
  ImGui::End();
}

void OpenGLBackend::render_node_gui(Scene& scene, const Entity entity)
{
  GRAVEL_ASSERT(scene.get_registry().all_of<Node>(entity));

  ImGui::PushID(static_cast<int>(entity));

  auto& registry = scene.get_registry();
  const auto& identifier = registry.get<Identifier>(entity);

  if (ImGui::CollapsingHeader(identifier.name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::Indent();

    if (auto* transform = registry.try_get<Transform>(entity)) {
      ImGui::SeparatorText("Transform");
      ImGui::DragFloat3("Translation", glm::value_ptr(transform->translation), 0.25f);
      ImGui::DragFloat3("Rotation", glm::value_ptr(transform->rotation));
      ImGui::DragFloat3("Scale",
                        glm::value_ptr(transform->scale),
                        1,
                        0.001f,
                        10'000,
                        "%.3f",
                        ImGuiSliderFlags_Logarithmic);
    }

    if (const auto* model = registry.try_get<gl::Model>(entity)) {
      ImGui::SeparatorText("OpenGLModel");
      ImGui::Text("Meshes: %zu", model->meshes.size());
    }

    ImGui::Spacing();

    const auto& node = registry.get<Node>(entity);
    for (const auto child_entity : node.children) {
      render_node_gui(scene, child_entity);
    }

    ImGui::Spacing();
    ImGui::Unindent();
  }

  ImGui::PopID();
}

}  // namespace gravel::gl