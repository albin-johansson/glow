#include "opengl_backend.hpp"

#include <cassert>  // assert
#include <cstdlib>  // offsetof

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>
#include <spdlog/spdlog.h>

#include "common/predef.hpp"
#include "graphics/opengl/model.hpp"
#include "graphics/opengl/util.hpp"
#include "io/texture_loader.hpp"
#include "scene/component/identifier.hpp"
#include "scene/component/node.hpp"
#include "scene/component/transform.hpp"
#include "scene/scene.hpp"

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

  mFramebufferProgram.load_shader_files(vert_path, frag_path)
      .check("[GL] Framebuffer program error");
  mFramebufferProgram.link().check("[GL] Framebuffer program link error");

  mFramebufferProgram.set_uniform("framebuffer", 0).check("[GL] Bad framebuffer binding");
}

void OpenGLBackend::load_environment_program()
{
  const char* vert_path = "assets/shaders/environment.vert";
  const char* frag_path = "assets/shaders/environment.frag";

  mEnvProgram.load_shader_files(vert_path, frag_path).check("[GL] Env program error");
  mEnvProgram.link().check("[GL] Env program link error");

  mEnvProgram.set_uniform("env_texture", 0).check("[GL] Bad env_texture binding");
  mEnvProgram.set_uniform_block_binding("UBO", 0).check("[GL] Bad UBO binding");
}

void OpenGLBackend::load_basic_program()
{
  const char* vert_path = "assets/shaders/basic.vert";
  const char* frag_path = "assets/shaders/basic.frag";

  mBasicProgram.load_shader_files(vert_path, frag_path).check("[GL] Basic program error");
  mBasicProgram.link().check("[GL] Basic program link error");

  mBasicProgram.set_uniform_block_binding("DynamicMatrices", 0)
      .check("[GL] Bad UBO binding");
}

void OpenGLBackend::init_uniform_buffers()
{
  mEnvProgramUbo.bind();
  mEnvProgramUbo.reserve_space(sizeof(EnvironmentOptions));

  mDynamicMatricesUbo.bind();
  mDynamicMatricesUbo.reserve_space(sizeof(DynamicMatrices));

  UniformBuffer::unbind();
}

void OpenGLBackend::load_environment_texture(const Path& path)
{
  const auto data =
      load_texture_data(path, TextureFormat::Float, TextureChannels::RGB).value();

  auto& env_texture = mEnvTexture.emplace();
  env_texture.bind();
  env_texture.set_data(0, GL_RGB32F, GL_RGB, GL_FLOAT, data.size, data.pixels.get());

  Texture2D::unbind();
}

void OpenGLBackend::update(const float dt)
{
  SDL_Event event {};
  while (SDL_PollEvent(&event)) {
    ImGui_ImplSDL2_ProcessEvent(&event);

    if (event.type == SDL_QUIT) {
      mQuit = true;
    }
    else if (event.type == SDL_KEYUP) {
      if (event.key.keysym.scancode == SDL_SCANCODE_H) {
        mHideUI = !mHideUI;
      }
    }
  }

  Vec2i mouse_pos {};
  SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);

  const auto& io = ImGui::GetIO();

  if (!io.WantCaptureKeyboard) {
    update_camera_position(dt);
  }

  if (!io.WantCaptureMouse) {
    update_camera_direction(dt);
  }

  Vec2i resolution {};
  SDL_GetWindowSizeInPixels(mWindow, &resolution.x, &resolution.y);

  mCamera.set_aspect_ratio(static_cast<float>(resolution.x) /
                           static_cast<float>(resolution.y));

  mLastMousePos = mouse_pos;
}

void OpenGLBackend::update_camera_position(const float dt)
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

void OpenGLBackend::update_camera_direction(const float dt)
{
  Vec2i mouse_pos {};
  const auto mouse_mask = SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
  const auto mouse_delta = mLastMousePos - Vec2 {mouse_pos};

  if (mouse_mask & SDL_BUTTON(SDL_BUTTON_LEFT)) {
    const auto yaw = mCameraSensitivity * mouse_delta.x * dt;
    const auto pitch = mCameraSensitivity * mouse_delta.y * dt;
    mCamera.rotate_direction(yaw, pitch);
  }
}

void OpenGLBackend::render(Scene& scene)
{
  const auto render_pass_start = Clock::now();

  ImGui_ImplSDL2_NewFrame();
  ImGui_ImplOpenGL3_NewFrame();
  ImGui::NewFrame();

  // Reset options
  set_option(GL_DEPTH_TEST, mDepthTest);
  set_option(GL_CULL_FACE, mCullFaces);
  set_option(GL_BLEND, mBlending);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Vec2i resolution {};
  SDL_GetWindowSizeInPixels(mWindow, &resolution.x, &resolution.y);

  // Update framebuffer dimensions
  mPrimaryBuffer.bind();
  mPrimaryBuffer.resize(resolution);

  mPrimaryBuffer.clear();

  const Mat4 projection = mCamera.to_projection_matrix();
  const Mat4 view = mCamera.to_view_matrix();

  // Render the environment backdrop
  render_environment(projection, view);
  glPolygonMode(GL_FRONT_AND_BACK, mWireframe ? GL_LINE : GL_FILL);

  render_models(scene, projection, view);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  render_buffer_to_screen(mPrimaryBuffer);

  if (!mHideUI) {
    render_gui(scene);
  }

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
  assert(get_bound_framebuffer() == mPrimaryBuffer.get_id());

  if (!mEnvTexture.has_value()) {
    return;
  }

  glActiveTexture(GL_TEXTURE0);
  mEnvTexture->bind();

  mEnvOptions.inverse_proj_view = glm::inverse(projection * view);
  mEnvOptions.camera_pos = Vec4 {mCamera.get_position(), 0};

  mEnvProgramUbo.bind();
  mEnvProgramUbo.update_data(0, sizeof mEnvOptions, &mEnvOptions);
  UniformBuffer::unbind();

  mEnvProgramUbo.bind_block(0);
  mEnvProgram.bind();
  mFullscreenQuad.draw_without_depth_test();

  Program::unbind();
  Texture2D::unbind();
  UniformBuffer::unbind_block(0);
}

void OpenGLBackend::render_models(Scene& scene, const Mat4& projection, const Mat4& view)
{
  mDynamicMatricesUbo.bind_block(0);
  mBasicProgram.bind();

  auto& registry = scene.get_registry();
  for (auto [entity, transform, model] :
       registry.view<comp::Transform, gl::Model>().each()) {
    const auto model_transform = transform.to_model_matrix();

    mDynamicMatricesUbo.bind();

    for (auto& mesh : model.meshes) {
      // TODO const auto& material = registry.get<comp::OpenGLMaterial>(mesh.material);

      mDynamicMatrices.m = model_transform * mesh.transform;
      mDynamicMatrices.mv = view * mDynamicMatrices.m;
      mDynamicMatrices.mvp = projection * mDynamicMatrices.mv;
      mDynamicMatrices.normal = glm::inverse(glm::transpose(mDynamicMatrices.mv));

      mDynamicMatricesUbo.update_data(0, sizeof mDynamicMatrices, &mDynamicMatrices);

      mesh.vao.bind();
      glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, nullptr);
    }

    VertexArray::unbind();
    UniformBuffer::unbind();
  }

  GRAVEL_GL_CHECK_ERRORS();

  Program::unbind();
  UniformBuffer::unbind_block(0);
  UniformBuffer::unbind_block(1);
}

void OpenGLBackend::render_gui(Scene& scene)
{
  if (ImGui::Begin("Information", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    const auto& io = ImGui::GetIO();
    ImGui::SeparatorText("Performance");
    ImGui::Text("FPS: %.2f", io.Framerate);
    ImGui::Text("Render pass: %.3f ms",
                static_cast<float64>(mRenderPassDuration.count()) / 1'000.0);
    if (ImGui::Checkbox("VSync", &mVSync)) {
      SDL_GL_SetSwapInterval(mVSync ? 1 : 0);
    }
    ImGui::Checkbox("Face culling", &mCullFaces);

    ImGui::SeparatorText("Debug");
    ImGui::Checkbox("Blending", &mBlending);
    ImGui::Checkbox("Wireframe", &mWireframe);
    ImGui::Checkbox("Depth test", &mDepthTest);

    Vec2i window_size {};
    SDL_GetWindowSize(mWindow, &window_size.x, &window_size.y);

    Vec2i resolution {};
    SDL_GetWindowSizeInPixels(mWindow, &resolution.x, &resolution.y);

    const auto& cam_pos = mCamera.get_position();
    const auto& cam_dir = mCamera.get_direction();
    ImGui::SeparatorText("Camera");
    ImGui::Text("Position: (%.1f, %.1f, %.1f)", cam_pos.x, cam_pos.y, cam_pos.z);
    ImGui::Text("Direction: (%.2f, %.2f, %.2f)", cam_dir.x, cam_dir.y, cam_dir.z);
    ImGui::Text("Aspect ratio: %.2f", mCamera.get_aspect_ratio());
    ImGui::SliderFloat("Speed",
                       &mCameraSpeed,
                       0.1f,
                       2000.0f,
                       "%.3f",
                       ImGuiSliderFlags_Logarithmic);
    ImGui::SliderFloat("Sensitivity", &mCameraSensitivity, 0.1f, 5.0f);

    ImGui::SeparatorText("Environment");

    bool use_gamma = mEnvOptions.use_gamma_correction;
    ImGui::Checkbox("Gamma correction", &use_gamma);
    mEnvOptions.use_gamma_correction = use_gamma;

    ImGui::SliderFloat("Gamma", &mEnvOptions.gamma, 1.0f, 4.0f);
    ImGui::SliderFloat("Brightness", &mEnvOptions.brightness, 0.0f, 1.0f, "%.2f");

    ImGui::SeparatorText("System");
    ImGui::TextUnformatted("API: OpenGL 4.1.0 core");
    ImGui::Text("Window size: (%d, %d)", window_size.x, window_size.y);
    ImGui::Text("Resolution: (%d, %d)", resolution.x, resolution.y);
  }
  ImGui::End();

  if (ImGui::Begin("Scene")) {
    const auto exclusions = entt::exclude<comp::ChildNode>;
    const auto view = scene.get_registry().view<comp::Node>(exclusions);
    for (auto [entity, node] : view.each()) {
      render_node_gui(scene, entity);
    }
  }
  ImGui::End();
}

void OpenGLBackend::render_node_gui(Scene& scene, const Entity entity)
{
  assert(scene.get_registry().all_of<comp::Node>(entity));

  ImGui::PushID(static_cast<int>(entity));

  auto& registry = scene.get_registry();
  const auto& identifier = registry.get<comp::Identifier>(entity);

  if (ImGui::CollapsingHeader(identifier.name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::Indent();

    if (auto* transform = registry.try_get<comp::Transform>(entity)) {
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

    const auto& node = registry.get<comp::Node>(entity);
    for (const auto child_entity : node.children) {
      render_node_gui(scene, child_entity);
    }

    ImGui::Spacing();
    ImGui::Unindent();
  }

  ImGui::PopID();
}

void OpenGLBackend::render_buffer_to_screen(const Framebuffer& buffer)
{
  Framebuffer::unbind();

  const auto& buffer_size = buffer.get_size();
  glViewport(0, 0, buffer_size.x, buffer_size.y);

  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  mFramebufferProgram.bind();

  glActiveTexture(GL_TEXTURE0);
  Texture2D::bind(buffer.get_color_texture_id());

  mFullscreenQuad.draw_without_depth_test();

  Program::unbind();
  Texture2D::unbind();
}

}  // namespace gravel::gl