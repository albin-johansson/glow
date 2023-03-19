#include "camera.hpp"

#include <glm/gtx/transform.hpp>

#include "scene/transform.hpp"

namespace gravel {

void rotate_camera(const Camera& camera,
                   Transform& transform,
                   const float32 yaw,
                   const float32 pitch)
{
  const Mat4 yaw_matrix = glm::rotate(yaw, camera.up);

  const Vec3 pitch_rotation_axis =
      glm::normalize(glm::cross(transform.rotation, camera.up));
  const Mat4 pitch_matrix = glm::rotate(pitch, pitch_rotation_axis);

  transform.rotation = Vec3 {pitch_matrix * yaw_matrix * Vec4 {transform.rotation, 0}};
}

auto to_projection_matrix(const Camera& camera) -> Mat4
{
  return glm::perspective(glm::radians(camera.fov),
                          camera.aspect_ratio,
                          camera.near_plane,
                          camera.far_plane);
}

auto to_view_matrix(const Camera& camera, const Transform& transform) -> Mat4
{
  return glm::lookAt(transform.position,
                     transform.position + transform.rotation,
                     camera.up);
}

}  // namespace gravel
