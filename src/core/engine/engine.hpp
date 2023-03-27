#pragma once

#include <SDL2/SDL.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/dispatcher.hpp"
#include "common/type/ecs.hpp"
#include "common/type/math.hpp"
#include "common/type/memory.hpp"
#include "common/type/path.hpp"
#include "engine/engine_initializer.hpp"
#include "graphics/graphics_api.hpp"
#include "scene/scene.hpp"
#include "ui/events.hpp"

namespace gravel {

GRAVEL_FORWARD_DECLARE_C(Backend);

class Engine final {
 public:
  explicit Engine(GraphicsApi api);

  void init();

  void start();

  void set_backend(Unique<Backend> backend);

  void set_environment_texture(const Path& path);

  void load_model(const Path& path);

  [[nodiscard]] auto get_window() -> SDL_Window* { return mWindow; }

 private:
  EngineInitializer mInitializer;
  SDL_Window* mWindow {};
  float64 mCounterFreq {};
  float64 mFixedDelta {};
  int32 mMaxTicksPerFrame {5};
  Unique<Backend> mBackend;
  Scene mScene;
  Dispatcher mDispatcher;
  Vec2i mViewportSize {};
  Vec2i mViewportResolution {};
  bool mRestoreLayout {true};
  bool mShowDemoWindow {false};
  bool mShowStyleEditor {false};

  void update(float32 dt);

  void update_camera_position(float32 dt);

  void render();

  void register_events();

  void on_quit(const QuitEvent&);

  void on_update_transform_translation(const UpdateTransformTranslationEvent& event);

  void on_update_transform_rotation(const UpdateTransformRotationEvent& event);

  void on_update_transform_scale(const UpdateTransformScaleEvent& event);

  void on_update_transform(const UpdateTransformEvent& event);

  void on_increase_font_size(const IncreaseFontSizeEvent&);

  void on_decrease_font_size(const DecreaseFontSizeEvent&);

  void on_reset_font_size(const ResetFontSizeEvent&);

  void on_set_gizmo_operation(const SetGizmoOperationEvent& event);

  void on_set_gizmo_mode(const SetGizmoModeEvent& event);

  void on_set_environment_brightness(const SetEnvironmentBrightnessEvent& event);
  void on_set_environment_gamma(const SetEnvironmentGammaEvent& event);
  void on_toggle_environment_gamma_correction(
      const ToggleEnvironmentGammaCorrectionEvent&);

  void on_rotate_active_camera(const RotateActiveCameraEvent& event);

  void on_set_camera_speed(const SetCameraSpeedEvent& event);

  void on_set_camera_sensitivity(const SetCameraSensitivityEvent& event);

  void on_set_camera_fov(const SetCameraFovEvent& event);

  void on_set_camera_near_plane(const SetCameraNearPlaneEvent& event);

  void on_set_camera_far_plane(const SetCameraFarPlaneEvent& event);

  void on_set_camera_aspect_ratio(const SetCameraAspectRatioEvent& event);

  void on_restore_layout(const RestoreLayoutEvent&);

  void on_show_style_editor(const ShowStyleEditorEvent&);

  void on_show_demo_window(const ShowDemoWindowEvent&);

  void on_toggle_rendering_option(const ToggleRenderingOptionEvent& event);

  [[nodiscard]] auto query_counter() const -> float64;
};

}  // namespace gravel
