#include "perspective_camera.hpp"

#include <glm/gtx/transform.hpp>

namespace gravel {

void PerspectiveCamera::rotate_direction(const float yaw, const float pitch)
{
  const Mat4 yaw_matrix = glm::rotate(yaw, mWorldUp);

  const Vec3 pitch_rotation_axis = glm::normalize(glm::cross(mDir, mWorldUp));
  const Mat4 pitch_matrix = glm::rotate(pitch, pitch_rotation_axis);

  mDir = Vec3 {pitch_matrix * yaw_matrix * Vec4 {mDir, 0}};
}

void PerspectiveCamera::offset_position(const Vec3& offset)
{
  mPos += offset;
}

void PerspectiveCamera::set_position(const Vec3& position)
{
  mPos = position;
}

void PerspectiveCamera::set_direction(const Vec3& direction)
{
  mDir = direction;
}

void PerspectiveCamera::set_world_up(const Vec3& up)
{
  mWorldUp = up;
}

void PerspectiveCamera::set_fov(const float fov)
{
  mFov = fov;
}

void PerspectiveCamera::set_aspect_ratio(const float ratio)
{
  mAspectRatio = ratio;
}

void PerspectiveCamera::set_near_plane(const float near_plane)
{
  mNearPlane = near_plane;
}

void PerspectiveCamera::set_far_plane(const float far_plane)
{
  mFarPlane = far_plane;
}

auto PerspectiveCamera::to_projection_matrix() const -> Mat4
{
  return glm::perspective(glm::radians(mFov), mAspectRatio, mNearPlane, mFarPlane);
}

auto PerspectiveCamera::to_view_matrix() const -> Mat4
{
  return glm::lookAt(mPos, mPos + mDir, mWorldUp);
}

}  // namespace gravel