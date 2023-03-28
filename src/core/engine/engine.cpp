#include "engine.hpp"

#include <algorithm>  // min
#include <utility>    // move

#include <imgui.h>
#include <imgui_internal.h>
#include <spdlog/spdlog.h>

#include "common/debug/error.hpp"
#include "common/type/math.hpp"
#include "engine/backend.hpp"
#include "graphics/camera.hpp"
#include "graphics/environment.hpp"
#include "graphics/rendering_options.hpp"
#include "scene/transform.hpp"
#include "ui/camera_options.hpp"
#include "ui/dock_space.hpp"
#include "ui/menu_bar.hpp"
#include "ui/scene_tree_dock.hpp"
#include "ui/scene_viewport.hpp"

namespace gravel {
namespace {

[[nodiscard]] auto determine_refresh_rate() -> float64
{
  SDL_DisplayMode mode {};
  if (SDL_GetDesktopDisplayMode(0, &mode) == 0) {
    return std::min(120, mode.refresh_rate);
  }
  else {
    spdlog::warn("[Engine] Failed to determine screen refresh rate, assuming 60 Hz");
    return 60;
  }
}

}  // namespace

Engine::Engine(const GraphicsApi api)
    : mInitializer {api},
      mWindow {mInitializer.get_window().get_handle()},
      mCounterFreq(static_cast<float64>(SDL_GetPerformanceFrequency())),
      mFixedDelta {1.0 / determine_refresh_rate()}
{
  spdlog::debug("[Engine] Counter frequency: {:L}", mCounterFreq);
  spdlog::debug("[Engine] Fixed delta: {:.4f}", mFixedDelta);
  spdlog::debug("[Engine] Max ticks per frame: {}", mMaxTicksPerFrame);

  SDL_MaximizeWindow(mWindow);
  SDL_ShowWindow(mWindow);

  register_events();
}

void Engine::register_events()
{
  // clang-format off
  mDispatcher.sink<QuitEvent>().connect<&Engine::on_quit>(this);

  mDispatcher.sink<UpdateTransformTranslationEvent>().connect<&Engine::on_update_transform_translation>(this);
  mDispatcher.sink<UpdateTransformRotationEvent>().connect<&Engine::on_update_transform_rotation>(this);
  mDispatcher.sink<UpdateTransformScaleEvent>().connect<&Engine::on_update_transform_scale>(this);
  mDispatcher.sink<UpdateTransformEvent>().connect<&Engine::on_update_transform>(this);

  mDispatcher.sink<IncreaseFontSizeEvent>().connect<&Engine::on_increase_font_size>(this);
  mDispatcher.sink<DecreaseFontSizeEvent>().connect<&Engine::on_decrease_font_size>(this);
  mDispatcher.sink<ResetFontSizeEvent>().connect<&Engine::on_reset_font_size>(this);

  mDispatcher.sink<SetGizmoOperationEvent>().connect<&Engine::on_set_gizmo_operation>(this);
  mDispatcher.sink<SetGizmoModeEvent>().connect<&Engine::on_set_gizmo_mode>(this);

  mDispatcher.sink<SetEnvironmentBrightnessEvent>().connect<&Engine::on_set_environment_brightness>(this);
  mDispatcher.sink<SetEnvironmentGammaEvent>().connect<&Engine::on_set_environment_gamma>(this);
  mDispatcher.sink<ToggleEnvironmentGammaCorrectionEvent>().connect<&Engine::on_toggle_environment_gamma_correction>(this);

  mDispatcher.sink<RotateActiveCameraEvent>().connect<&Engine::on_rotate_active_camera>(this);
  mDispatcher.sink<SetCameraSpeedEvent>().connect<&Engine::on_set_camera_speed>(this);
  mDispatcher.sink<SetCameraSensitivityEvent>().connect<&Engine::on_set_camera_sensitivity>(this);

  mDispatcher.sink<SetCameraFovEvent>().connect<&Engine::on_set_camera_fov>(this);
  mDispatcher.sink<SetCameraNearPlaneEvent>().connect<&Engine::on_set_camera_near_plane>(this);
  mDispatcher.sink<SetCameraFarPlaneEvent>().connect<&Engine::on_set_camera_far_plane>(this);
  mDispatcher.sink<SetCameraAspectRatioEvent>().connect<&Engine::on_set_camera_aspect_ratio>(this);

  mDispatcher.sink<RestoreLayoutEvent>().connect<&Engine::on_restore_layout>(this);
  mDispatcher.sink<ShowStyleEditorEvent>().connect<&Engine::on_show_style_editor>(this);
  mDispatcher.sink<ShowDemoWindowEvent>().connect<&Engine::on_show_demo_window>(this);

  mDispatcher.sink<ToggleRenderingOptionEvent>().connect<&Engine::on_toggle_rendering_option>(this);
  // clang-format on
}

void Engine::init()
{
  if (!mBackend) {
    throw Error {"[Engine] Backend instance was not set"};
  }

  mBackend->on_init(mScene);
}

void Engine::start()
{
  if (!mBackend) {
    throw Error {"[Engine] Backend instance was not set"};
  }

  auto last_update = query_counter();
  Vec2 last_framebuffer_scale {};

  // The following is a semi-fixed delta time game loop implementation.
  // Logic updates are detached from rendered frames, that is, there may be multiple
  // logic updates for each render call.
  while (!mBackend->should_quit()) {
    const auto new_time = query_counter();

    auto frame_time = new_time - last_update;
    last_update = new_time;

    int32 ticks = 0;
    while (frame_time > 0) {
      if (ticks > mMaxTicksPerFrame) {
        // Avoids spiral of death
        break;
      }

      if (mBackend->should_quit()) {
        break;
      }

      const auto delta = std::min(frame_time, mFixedDelta);
      update(static_cast<float32>(delta));

      frame_time -= delta;
      ++ticks;
    }

    const auto& io = ImGui::GetIO();
    const Vec2 fb_scale {io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y};

    if (fb_scale != last_framebuffer_scale) {
      mInitializer.get_imgui().reload_fonts();
      last_framebuffer_scale = fb_scale;
    }

    render();
  }

  mBackend->on_quit();

  SDL_HideWindow(mWindow);
}

void Engine::update(const float32 dt)
{
  update_camera_position(dt);

  SDL_Event event {};
  while (SDL_PollEvent(&event)) {
    mBackend->on_event(event);
  }

  mDispatcher.update();
}

void Engine::update_camera_position(const float32 dt)
{
  const uint8* state = SDL_GetKeyboardState(nullptr);

  const auto& camera_context = mScene.get<CameraContext>();
  const auto& camera_options = mScene.get<CameraOptions>();

  const auto& camera = mScene.get<Camera>(camera_context.active_camera);
  auto& transform = mScene.get<Transform>(camera_context.active_camera);

  if (state[SDL_SCANCODE_W]) {
    transform.position += transform.rotation * camera_options.speed * dt;
  }
  else if (state[SDL_SCANCODE_S]) {
    transform.position += -transform.rotation * camera_options.speed * dt;
  }

  if (state[SDL_SCANCODE_A]) {
    const auto cross = glm::cross(transform.rotation, camera.up);
    transform.position += -cross * camera_options.speed * dt;
  }
  else if (state[SDL_SCANCODE_D]) {
    const auto cross = glm::cross(transform.rotation, camera.up);
    transform.position += cross * camera_options.speed * dt;
  }

  if (state[SDL_SCANCODE_E]) {
    transform.position += camera.up * camera_options.speed * dt;
  }
  else if (state[SDL_SCANCODE_Q]) {
    transform.position += -camera.up * camera_options.speed * dt;
  }
}

void Engine::render()
{
  if (mBackend->begin_frame().failed()) {
    spdlog::error("[Engine] Failed to begin frame");
    return;
  }

  update_dock_space(mRestoreLayout);

  show_menu_bar(mScene, mDispatcher);
  show_scene_viewport(mScene, *mBackend, mDispatcher);
  show_scene_tree_dock(mScene, mDispatcher);

  if (mShowStyleEditor) {
    if (ImGui::Begin("Style Editor")) {
      ImGui::ShowStyleEditor();
    }
    ImGui::End();
  }

  if (mShowDemoWindow) {
    ImGui::ShowDemoWindow();
  }

  mRestoreLayout = false;
  mShowStyleEditor = false;
  mShowDemoWindow = false;

  mBackend->end_frame();
}

void Engine::on_quit(const QuitEvent&)
{
  mBackend->stop();
}

void Engine::on_update_transform_translation(const UpdateTransformTranslationEvent& event)
{
  spdlog::trace("UpdateTransformTranslationEvent");

  auto& transform = mScene.get<Transform>(event.transform_entity);
  transform.position = event.translation;
}

void Engine::on_update_transform_rotation(const UpdateTransformRotationEvent& event)
{
  spdlog::trace("UpdateTransformRotationEvent");

  auto& transform = mScene.get<Transform>(event.transform_entity);
  transform.rotation = event.rotation;
}

void Engine::on_update_transform_scale(const UpdateTransformScaleEvent& event)
{
  spdlog::trace("UpdateTransformScaleEvent");

  auto& transform = mScene.get<Transform>(event.transform_entity);
  transform.scale = event.scale;
}

void Engine::on_update_transform(const UpdateTransformEvent& event)
{
  spdlog::trace("UpdateTransformEvent");

  auto& transform = mScene.get<Transform>(event.transform_entity);
  transform.position = event.translation;
  transform.rotation = event.rotation;
  transform.scale = event.scale;
}

void Engine::on_increase_font_size(const IncreaseFontSizeEvent&)
{
  spdlog::trace("IncreaseFontSizeEvent");

  mInitializer.get_imgui().increase_font_size();
}

void Engine::on_decrease_font_size(const DecreaseFontSizeEvent&)
{
  spdlog::trace("DecreaseFontSizeEvent");

  mInitializer.get_imgui().decrease_font_size();
}

void Engine::on_reset_font_size(const ResetFontSizeEvent&)
{
  spdlog::trace("ResetFontSizeEvent");

  mInitializer.get_imgui().reset_font_size();
}

void Engine::on_set_gizmo_operation(const SetGizmoOperationEvent& event)
{
  spdlog::trace("SetGizmoOperationEvent");

  auto& gizmos_options = mScene.get<GizmosOptions>();
  gizmos_options.operation = event.operation;
}

void Engine::on_set_gizmo_mode(const SetGizmoModeEvent& event)
{
  spdlog::trace("SetGizmoModeEvent");

  auto& gizmos_options = mScene.get<GizmosOptions>();
  gizmos_options.mode = event.mode;
}

void Engine::on_set_environment_brightness(const SetEnvironmentBrightnessEvent& event)
{
  spdlog::trace("SetEnvironmentBrightnessEvent");

  auto& env_options = mScene.get<EnvironmentOptions>();
  env_options.brightness = event.brightness;
}

void Engine::on_set_environment_gamma(const SetEnvironmentGammaEvent& event)
{
  spdlog::trace("SetEnvironmentGammaEvent");

  auto& env_options = mScene.get<EnvironmentOptions>();
  env_options.gamma = event.gamma;
}

void Engine::on_toggle_environment_gamma_correction(
    const ToggleEnvironmentGammaCorrectionEvent&)
{
  spdlog::trace("ToggleEnvironmentGammaCorrectionEvent");

  auto& env_options = mScene.get<EnvironmentOptions>();
  env_options.use_gamma_correction = !env_options.use_gamma_correction;
}

void Engine::on_rotate_active_camera(const RotateActiveCameraEvent& event)
{
  spdlog::trace("RotateCameraEvent");

  auto [camera_entity, camera] = mScene.get_active_camera();
  auto& transform = mScene.get<Transform>(camera_entity);

  rotate_camera(camera, transform, event.yaw, event.pitch);
}

void Engine::on_set_camera_speed(const SetCameraSpeedEvent& event)
{
  spdlog::trace("SetCameraSpeedEvent");

  auto& camera_options = mScene.get<CameraOptions>();
  camera_options.speed = event.speed;
}

void Engine::on_set_camera_sensitivity(const SetCameraSensitivityEvent& event)
{
  spdlog::trace("SetCameraSensitivityEvent");

  auto& camera_options = mScene.get<CameraOptions>();
  camera_options.sensitivity = event.sensitivity;
}

void Engine::on_set_camera_fov(const SetCameraFovEvent& event)
{
  spdlog::trace("SetCameraFovEvent");

  auto& camera = mScene.get<Camera>(event.camera_entity);
  camera.fov = event.fov;
}

void Engine::on_set_camera_near_plane(const SetCameraNearPlaneEvent& event)
{
  spdlog::trace("SetCameraNearPlaneEvent");

  auto& camera = mScene.get<Camera>(event.camera_entity);
  camera.near_plane = event.near_plane;
}

void Engine::on_set_camera_far_plane(const SetCameraFarPlaneEvent& event)
{
  spdlog::trace("SetCameraFarPlaneEvent");

  auto& camera = mScene.get<Camera>(event.camera_entity);
  camera.far_plane = event.far_plane;
}

void Engine::on_set_camera_aspect_ratio(const SetCameraAspectRatioEvent& event)
{
  spdlog::trace("SetCameraAspectRatioEvent");

  auto& camera = mScene.get<Camera>(event.camera_entity);
  camera.aspect_ratio = event.aspect_ratio;
}

void Engine::on_restore_layout(const RestoreLayoutEvent&)
{
  spdlog::trace("RestoreLayoutEvent");
  mRestoreLayout = true;
}

void Engine::on_show_style_editor(const ShowStyleEditorEvent&)
{
  spdlog::trace("ShowStyleEditorEvent");
  mShowStyleEditor = true;
}

void Engine::on_show_demo_window(const ShowDemoWindowEvent&)
{
  spdlog::trace("ShowDemoWindowEvent");
  mShowDemoWindow = true;
}

void Engine::on_toggle_rendering_option(const ToggleRenderingOptionEvent& event)
{
  spdlog::trace("ToggleRenderingOptionEvent");

  auto& rendering_options = mScene.get<RenderingOptions>();
  auto& value = rendering_options.options.at(event.option);
  value = !value;
}

void Engine::set_backend(Unique<Backend> backend)
{
  mBackend = std::move(backend);
}

void Engine::set_environment_texture(const Path& path)
{
  mBackend->set_environment_texture(mScene, path);
}

void Engine::load_model(const Path& path)
{
  mBackend->load_model(mScene, path);
}

auto Engine::query_counter() const -> float64
{
  return static_cast<float64>(SDL_GetPerformanceCounter()) / mCounterFreq;
}

}  // namespace gravel