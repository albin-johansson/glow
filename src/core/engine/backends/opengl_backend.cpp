#include "opengl_backend.hpp"

#include <fmt/format.h>
#include <glad/glad.h>
#include <imgui_impl_sdl2.h>

// This comment prevents moving the following include before <imgui.h>
#include <ImGuizmo.h>

#include "common/debug/assert.hpp"
#include "common/predef.hpp"
#include "graphics/camera.hpp"
#include "graphics/environment.hpp"
#include "graphics/opengl/model.hpp"
#include "graphics/opengl/texture_cache.hpp"
#include "graphics/opengl/util.hpp"
#include "graphics/renderer_info.hpp"
#include "graphics/rendering_options.hpp"
#include "init/window.hpp"
#include "io/texture_loader.hpp"
#include "scene/scene.hpp"
#include "scene/transform.hpp"
#include "ui/camera_options.hpp"
#include "ui/events.hpp"
#include "ui/gizmos.hpp"
#include "util/bits.hpp"

namespace glow {

OpenGLBackend::OpenGLBackend(SDL_Window* window)
    : mRenderer {window}
{
}

void OpenGLBackend::stop()
{
  mQuit = true;
}

void OpenGLBackend::on_init(Scene& scene)
{
  scene.add<gl::TextureCache>();

  auto& rendering_options = scene.get<RenderingOptions>();
  rendering_options.options = {
      {RenderingOption::VSync, true},
      {RenderingOption::DepthTest, true},
      {RenderingOption::FaceCulling, true},
      {RenderingOption::Wireframe, false},
      {RenderingOption::Blending, false},
  };

  auto& renderer_info = scene.get<RendererInfo>();
  renderer_info.api = "OpenGL 4.1.0 core";
  renderer_info.renderer = gl::get_renderer_name();
  renderer_info.vendor = gl::get_vendor_name();
  renderer_info.version = gl::get_version();

  const auto camera_entity =
      make_camera(scene, "Camera", Vec3 {0, 0, 3}, Vec3 {0, 0, -1});

  auto& camera_context = scene.get<CameraContext>();
  camera_context.active_camera = camera_entity;
}

void OpenGLBackend::on_event(const SDL_Event& event)
{
  ImGui_ImplSDL2_ProcessEvent(&event);

  if (event.type == SDL_QUIT) {
    mQuit = true;
  }
}

auto OpenGLBackend::begin_frame(const Scene& scene) -> Result
{
  mRenderer.begin_frame();
  return kSuccess;
}

void OpenGLBackend::end_frame()
{
  mRenderer.end_frame();
  mRenderer.swap_buffers();
}

void OpenGLBackend::render_scene(const Scene& scene, Dispatcher& dispatcher)
{
  // Reset options
  const auto& rendering_options = scene.get<RenderingOptions>();
  gl::set_option(GL_DEPTH_TEST, rendering_options.test(RenderingOption::DepthTest));
  gl::set_option(GL_CULL_FACE, rendering_options.test(RenderingOption::FaceCulling));
  gl::set_option(GL_BLEND, rendering_options.test(RenderingOption::Blending));
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  SDL_GL_SetSwapInterval(rendering_options.test(RenderingOption::VSync) ? 1 : 0);

  mOffscreenFB.bind();

  Vec2i window_resolution {};
  SDL_GetWindowSizeInPixels(get_window(), &window_resolution.x, &window_resolution.y);

  if (window_resolution != mOffscreenFB.get_size()) {
    mOffscreenFB.resize(window_resolution);
  }

  mOffscreenFB.clear();

  if (scene.has_active_camera()) {
    const auto& [camera_entity, camera] = scene.get_active_camera();
    const auto& camera_transform = scene.get<Transform>(camera_entity);

    const Vec2 fb_size = mOffscreenFB.get_size();
    const auto fb_aspect_ratio = fb_size.x / fb_size.y;

    if (camera.aspect_ratio != fb_aspect_ratio) {
      dispatcher.enqueue<SetCameraAspectRatioEvent>(camera_entity, fb_aspect_ratio);
    }

    const auto projection = to_projection_matrix(camera, GraphicsAPI::OpenGL);
    const auto view = to_view_matrix(camera, camera_transform, GraphicsAPI::OpenGL);

    // Render the environment backdrop
    // TODO optimization: render environment last with depth test trickery
    render_environment(scene, projection, view, camera_entity);

    // Render the scene objects
    glPolygonMode(GL_FRONT_AND_BACK,
                  rendering_options.test(RenderingOption::Wireframe) ? GL_LINE : GL_FILL);
    render_models(scene, projection, view, dispatcher);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    gl::Framebuffer::unbind();
  }

  auto& present_options = mRenderer.get_framebuffer_program_options_buffer();
  present_options.gamma = 2.2f;
  present_options.gamma_correction_enabled = GL_TRUE;

  mRenderer.render_buffer_to_screen(mOffscreenFB);
  GLOW_GL_CHECK_ERRORS();
}

void OpenGLBackend::render_environment(const Scene& scene,
                                       const Mat4& projection,
                                       const Mat4& view,
                                       const Entity camera_entity)
{
  if (mEnvTexture.has_value()) {
    const auto& camera_position = scene.get<Transform>(camera_entity).position;
    const auto& env_options = scene.get<EnvironmentOptions>();

    auto& env_buffer = mRenderer.get_env_buffer();
    env_buffer.brightness = env_options.brightness;
    env_buffer.gamma = env_options.gamma;
    env_buffer.use_gamma_correction = env_options.use_gamma_correction;
    env_buffer.camera_pos = Vec4 {camera_position, 0};
    env_buffer.inverse_proj_view = glm::inverse(projection * view);

    mRenderer.render_environment(*mEnvTexture);
  }
}

void OpenGLBackend::render_models(const Scene& scene,
                                  const Mat4& projection,
                                  const Mat4& view,
                                  Dispatcher& dispatcher)
{
  mRenderer.bind_shading_program();

  const auto& gizmos_options = scene.get<GizmosOptions>();

  for (auto [entity, transform, model] : scene.each<Transform, gl::Model>()) {
    const auto model_transform = transform.to_model_matrix();

    for (const auto& mesh : model.meshes) {
      const auto& material = scene.get<gl::Material>(mesh.material);
      const auto model_matrix = model_transform * mesh.transform;

      auto& matrix_buffer = mRenderer.get_matrix_buffer();
      matrix_buffer.m = model_matrix;
      matrix_buffer.mv = view * matrix_buffer.m;
      matrix_buffer.mvp = projection * matrix_buffer.mv;
      matrix_buffer.normal = glm::inverse(glm::transpose(matrix_buffer.mv));

      auto& material_buffer = mRenderer.get_material_buffer();
      material_buffer.ambient = Vec4 {material.ambient, 0};
      material_buffer.diffuse = Vec4 {material.diffuse, 0};
      material_buffer.specular = Vec4 {material.specular, 0};
      material_buffer.emission = Vec4 {material.emission, 0};
      material_buffer.has_diffuse_tex = material.diffuse_tex.has_value();
      material_buffer.has_specular_tex = material.specular_tex.has_value();

      mRenderer.render_shaded_mesh(mesh, material);
    }

    ImGuizmo::SetID(static_cast<int>(entity));
    show_model_control_gizmo(gizmos_options,
                             projection,
                             view,
                             entity,
                             transform,
                             dispatcher);
  }

  GLOW_GL_CHECK_ERRORS();

  mRenderer.unbind_shading_program();
}

void OpenGLBackend::set_environment_texture([[maybe_unused]] Scene& scene,
                                            const Path& path)
{
  mEnvTexture = gl::Texture2D::load_rgb_f32(path);
}

void OpenGLBackend::load_model(Scene& scene, const Path& path)
{
  static int index = 0;

  const auto model_entity = scene.make_node(fmt::format("Model {}", index));
  gl::assign_model(scene, model_entity, path);

  ++index;
}

}  // namespace glow