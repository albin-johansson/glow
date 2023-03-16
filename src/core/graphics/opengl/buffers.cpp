#include "buffers.hpp"

#include "graphics/opengl/model.hpp"

namespace gravel::gl {

void MatrixBuffer::update(const Mat4& model, const Mat4& view, const Mat4& projection)
{
  m = model;
  mv = view * model;
  mvp = projection * mv;
  normal = glm::inverse(glm::transpose(mv));
}

void MaterialBuffer::update(const Material& material)
{
  ambient = Vec4 {material.ambient, 0};
  diffuse = Vec4 {material.diffuse, 0};
  specular = Vec4 {material.specular, 0};
  emission = Vec4 {material.emission, 0};

  has_diffuse_tex = material.diffuse_tex.has_value();
  has_specular_tex = material.specular_tex.has_value();
}

}  // namespace gravel::gl
