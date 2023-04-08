#include "camera.hpp"

#include <utility>  // move

#include <glm/gtx/transform.hpp>

#include "scene/scene.hpp"
#include "scene/transform.hpp"

namespace gravel {

auto make_camera(Scene& scene, String name, const Vec3& position, const Vec3& direction)
    -> Entity
{
  const auto camera_entity = scene.make_node(std::move(name));

  auto& camera = scene.add<Camera>(camera_entity);
  camera.up = Vec3 {0, 1, 0};

  auto& transform = scene.get<Transform>(camera_entity);
  transform.position = position;
  transform.rotation = direction;

  auto& transform_options = scene.get<TransformOptions>(camera_entity);
  transform_options.use_rotation = false;
  transform_options.use_scale = false;

  return camera_entity;
}

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

auto to_projection_matrix(const Camera& camera, const GraphicsApi api) -> Mat4
{
  if (api == GraphicsApi::Vulkan) {
    auto projection = glm::perspectiveLH(glm::radians(camera.fov),
                                         camera.aspect_ratio,
                                         camera.near_plane,
                                         camera.far_plane);
    projection[1][1] *= -1.0f;
    return projection;
  }
  else {
    return glm::perspective(glm::radians(camera.fov),
                            camera.aspect_ratio,
                            camera.near_plane,
                            camera.far_plane);
  }
}

auto to_view_matrix(const Camera& camera,
                    const Transform& transform,
                    const GraphicsApi api) -> Mat4
{
  if (api == GraphicsApi::Vulkan) {
    return glm::lookAtLH(transform.position,
                         transform.position + transform.rotation,
                         camera.up);
  }
  else {
    return glm::lookAt(transform.position,
                       transform.position + transform.rotation,
                       camera.up);
  }
}

}  // namespace gravel
