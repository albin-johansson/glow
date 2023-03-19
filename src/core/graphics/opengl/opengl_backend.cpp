#include "opengl_backend.hpp"

#include <glad/glad.h>
#include <imgui_impl_sdl2.h>

// This comment prevents moving the following include before <imgui.h>
#include <ImGuizmo.h>

#include "common/debug/assert.hpp"
#include "common/predef.hpp"
#include "graphics/opengl/model.hpp"
#include "graphics/opengl/texture_cache.hpp"
#include "graphics/opengl/util.hpp"
#include "graphics/rendering_options.hpp"
#include "io/texture_loader.hpp"
#include "scene/camera.hpp"
#include "scene/scene.hpp"
#include "scene/transform.hpp"
#include "ui/camera_options.hpp"
#include "ui/events.hpp"
#include "ui/gizmos.hpp"
#include "util/bits.hpp"

namespace gravel::gl {

OpenGLBackend::OpenGLBackend(SDL_Window* window)
    : mRenderer {window}
{
}

void OpenGLBackend::load_environment_texture(const Path& path)
{
  mEnvTexture = Texture2D::load_rgb_f32(path);
}

void OpenGLBackend::stop()
{
  mQuit = true;
}

void OpenGLBackend::on_init(Scene& scene)
{
  auto& registry = scene.get_registry();

  auto& ctx = registry.ctx();
  ctx.emplace<CameraContext>();
  ctx.emplace<CameraOptions>();
  ctx.emplace<TextureCache>();
  ctx.emplace<GizmosOptions>();

  auto& rendering_options = ctx.emplace<RenderingOptions>();
  rendering_options.options = {
      {RenderingOption::VSync, true},
      {RenderingOption::DepthTest, true},
      {RenderingOption::FaceCulling, true},
      {RenderingOption::Wireframe, false},
      {RenderingOption::Blending, false},
  };

  make_main_camera_node(scene);


}

void OpenGLBackend::make_main_camera_node(Scene& scene)
{
  auto& registry = scene.get_registry();

  const auto camera_entity = scene.make_node("Main Camera");

  auto& camera = registry.emplace<Camera>(camera_entity);
  camera.up = Vec3 {0, 1, 0};

  auto& transform = registry.get<Transform>(camera_entity);
  transform.position = Vec3 {0, 0, 2};
  transform.rotation = Vec3 {0, 0, -1};

  auto& transform_options = registry.get<TransformOptions>(camera_entity);
  transform_options.use_rotation = false;
  transform_options.use_scale = false;

  auto& camera_context = registry.ctx().get<CameraContext>();
  camera_context.active_camera = camera_entity;
}

void OpenGLBackend::on_event(const SDL_Event& event)
{
  ImGui_ImplSDL2_ProcessEvent(&event);

  if (event.type == SDL_QUIT) {
    mQuit = true;
  }
}

void OpenGLBackend::begin_frame()
{
  mRenderer.begin_frame();
}

void OpenGLBackend::end_frame()
{
  mRenderer.end_frame();
  mRenderer.swap_buffers();
}

void OpenGLBackend::render_scene(const Scene& scene,
                                 const Vec2& framebuffer_size,
                                 Dispatcher& dispatcher)
{
  const auto& registry = scene.get_registry();

  // Reset options
  const auto& rendering_options = registry.ctx().get<RenderingOptions>();
  set_option(GL_DEPTH_TEST, rendering_options.test(RenderingOption::DepthTest));
  set_option(GL_CULL_FACE, rendering_options.test(RenderingOption::FaceCulling));
  set_option(GL_BLEND, rendering_options.test(RenderingOption::Blending));
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  SDL_GL_SetSwapInterval(rendering_options.test(RenderingOption::VSync) ? 1 : 0);

  // Update framebuffer dimensions
  mPrimaryFBO.bind();
  mPrimaryFBO.resize(framebuffer_size);
  mPrimaryFBO.clear();

  const auto aspect_ratio = framebuffer_size.x / framebuffer_size.y;

  if (has_active_camera(registry)) {
    const auto& [camera_entity, camera] = get_active_camera(registry);
    const auto& camera_transform = registry.get<Transform>(camera_entity);

    if (camera.aspect_ratio != aspect_ratio) {
      dispatcher.enqueue<SetCameraAspectRatioEvent>(camera_entity, aspect_ratio);
    }

    const auto projection = to_projection_matrix(camera);
    const auto view = to_view_matrix(camera, camera_transform);

    // Render the environment backdrop
    if (mEnvTexture.has_value()) {
      const auto& camera_position = registry.get<Transform>(camera_entity).position;
      mRenderer.render_environment(*mEnvTexture, projection, view, camera_position);
    }

    // Render the scene objects
    glPolygonMode(GL_FRONT_AND_BACK,
                  rendering_options.test(RenderingOption::Wireframe) ? GL_LINE : GL_FILL);
    render_models(scene, projection, view, dispatcher);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    Framebuffer::unbind();
    GRAVEL_GL_CHECK_ERRORS();
  }
}

void OpenGLBackend::render_models(const Scene& scene,
                                  const Mat4& projection,
                                  const Mat4& view,
                                  Dispatcher& dispatcher)
{
  mRenderer.bind_shading_program();

  const auto& registry = scene.get_registry();
  const auto& gizmos_options = registry.ctx().get<GizmosOptions>();

  for (auto [entity, transform, model] : registry.view<Transform, Model>().each()) {
    const auto model_transform = transform.to_model_matrix();

    for (const auto& mesh : model.meshes) {
      const auto& material = registry.get<Material>(mesh.material);

      const auto model_matrix = model_transform * mesh.transform;
      mRenderer.render_shaded_mesh(mesh, material, model_matrix, view, projection);
    }

    ImGuizmo::SetID(static_cast<int>(entity));
    show_model_control_gizmo(gizmos_options,
                             projection,
                             view,
                             entity,
                             transform,
                             dispatcher);
  }

  GRAVEL_GL_CHECK_ERRORS();

  mRenderer.unbind_shading_program();
}

auto OpenGLBackend::get_primary_framebuffer_handle() -> void*
{
  const auto id = static_cast<uintptr>(mPrimaryFBO.get_id());
  return bitcast<void*>(id);
}

}  // namespace gravel::gl