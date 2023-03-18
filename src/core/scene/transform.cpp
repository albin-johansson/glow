#include "transform.hpp"

#include <glm/gtx/transform.hpp>

namespace gravel {

auto Transform::to_model_matrix() const -> Mat4
{
  Mat4 matrix {1};

  matrix = glm::translate(matrix, position);
  matrix = glm::rotate(matrix, glm::radians(rotation.x), Vec3 {1, 0, 0});
  matrix = glm::rotate(matrix, glm::radians(rotation.y), Vec3 {0, 1, 0});
  matrix = glm::rotate(matrix, glm::radians(rotation.z), Vec3 {0, 0, 1});
  matrix = glm::scale(matrix, scale);

  return matrix;
}

}  // namespace gravel