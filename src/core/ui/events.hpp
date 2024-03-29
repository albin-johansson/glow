#pragma once

#include "common/primitives.hpp"
#include "common/type/ecs.hpp"
#include "common/type/math.hpp"
#include "graphics/rendering_options.hpp"
#include "ui/gizmos.hpp"

namespace glow {

struct QuitEvent final {};
struct RestoreLayoutEvent final {};
struct ShowStyleEditorEvent final {};
struct ShowDemoWindowEvent final {};

struct IncreaseFontSizeEvent final {};
struct DecreaseFontSizeEvent final {};
struct ResetFontSizeEvent final {};

struct SetGizmoOperationEvent final {
  GizmoOperation operation {};
};

struct SetGizmoModeEvent final {
  GizmoMode mode {};
};

struct SetEnvironmentBrightnessEvent final {
  float32 brightness {};
};

struct SetEnvironmentGammaEvent final {
  float32 gamma {};
};

struct ToggleEnvironmentGammaCorrectionEvent final {};

struct RotateActiveCameraEvent final {
  float32 yaw {};
  float32 pitch {};
};

struct SetCameraSpeedEvent final {
  float32 speed {};
};

struct SetCameraSensitivityEvent final {
  float32 sensitivity {};
};

struct SetCameraAspectRatioEvent final {
  Entity camera_entity {kNullEntity};
  float32 aspect_ratio {};
};

struct SetCameraFovEvent final {
  Entity camera_entity {kNullEntity};
  float32 fov {};
};

struct SetCameraNearPlaneEvent final {
  Entity camera_entity {kNullEntity};
  float32 near_plane {};
};

struct SetCameraFarPlaneEvent final {
  Entity camera_entity {kNullEntity};
  float32 far_plane {};
};

struct UpdateTransformTranslationEvent final {
  Entity transform_entity {kNullEntity};
  Vec3 translation {};
};

struct UpdateTransformRotationEvent final {
  Entity transform_entity {kNullEntity};
  Vec3 rotation {};
};

struct UpdateTransformScaleEvent final {
  Entity transform_entity {kNullEntity};
  Vec3 scale {};
};

struct UpdateTransformEvent final {
  Entity transform_entity {kNullEntity};
  Vec3 translation {};
  Vec3 rotation {};
  Vec3 scale {};
};

struct ToggleRenderingOptionEvent final {
  RenderingOption option {};
};

}  // namespace glow
