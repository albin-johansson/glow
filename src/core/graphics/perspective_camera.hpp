#pragma once

#include "common/type/math.hpp"

namespace gravel {

class PerspectiveCamera final {
 public:
  void rotate_direction(float yaw, float pitch);

  void offset_position(const Vec3& offset);

  void set_position(const Vec3& position);

  void set_direction(const Vec3& direction);

  void set_world_up(const Vec3& up);

  void set_fov(float fov);

  void set_aspect_ratio(float ratio);

  void set_near_plane(float near_plane);

  void set_far_plane(float far_plane);

  [[nodiscard]] auto get_position() const -> const Vec3&;

  [[nodiscard]] auto get_direction() const -> const Vec3&;

  [[nodiscard]] auto get_world_up() const -> const Vec3&;

  [[nodiscard]] auto get_fov() const -> float;

  [[nodiscard]] auto get_aspect_ratio() const -> float;

  [[nodiscard]] auto get_near_plane() const -> float;

  [[nodiscard]] auto get_far_plane() const -> float;

  [[nodiscard]] auto to_projection_matrix() const -> Mat4;

  [[nodiscard]] auto to_view_matrix() const -> Mat4;

 private:
  Vec3 mPos {};
  Vec3 mDir {0, 0, -1};
  Vec3 mWorldUp {0, 1, 0};
  float mFov {90};                    ///< Vertical field-of-view, in degrees.
  float mAspectRatio {16.0f / 9.0f};  ///< Far plane aspect ratio,
  float mNearPlane {0.1f};            ///< Distance to near plane.
  float mFarPlane {100'000};          ///< Distance to far plane.
};

}  // namespace gravel
