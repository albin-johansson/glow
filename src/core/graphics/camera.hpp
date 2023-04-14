#pragma once

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/ecs.hpp"
#include "common/type/math.hpp"
#include "common/type/string.hpp"
#include "graphics/graphics_api.hpp"

namespace glow {

GRAVEL_FORWARD_DECLARE_S(Transform);
GRAVEL_FORWARD_DECLARE_C(Scene);

/// Context component for tracking the active camera.
struct CameraContext final {
  Entity active_camera {kNullEntity};
};

/// Component for a perspective camera.
/// The position and direction are delegated to the transform component.
struct Camera final {
  Vec3 up {0, 1, 0};                    ///< Normalized up direction.
  float32 fov {90};                     ///< Vertical field-of-view, in degrees.
  float32 near_plane {0.1f};            ///< Distance to frustum near plane.
  float32 far_plane {100'000};          ///< Distance to frustum far plane.
  float32 aspect_ratio {16.0f / 9.0f};  ///< Far plane aspect ratio,
};

auto make_camera(Scene& scene, String name, const Vec3& position, const Vec3& direction)
    -> Entity;

/// Rotates a camera with the specified amount along the yaw and pitch axes.
///
/// \param camera the camera component.
/// \param transform the transform component associated with the camera.
/// \param yaw the rotation along the yaw axis, in radians.
/// \param pitch the rotation along the pitch axis, in radians.
void rotate_camera(const Camera& camera,
                   Transform& transform,
                   float32 yaw,
                   float32 pitch);

/// Creates a projection matrix based on a camera.
///
/// \param camera the camera component.
/// \param api the graphics API to adjust the matrix for.
/// \return a projection matrix.
[[nodiscard]] auto to_projection_matrix(const Camera& camera, GraphicsApi api) -> Mat4;

/// Creates a view matrix based on a camera.
///
/// \param camera the camera component.
/// \param transform the transform component associated with the camera.
/// \param api the graphics API to adjust the matrix for.
/// \return a view matrix.
[[nodiscard]] auto to_view_matrix(const Camera& camera,
                                  const Transform& transform,
                                  GraphicsApi api) -> Mat4;

}  // namespace glow
