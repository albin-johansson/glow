#include "quad.hpp"

#include <cstddef>  // offsetof

#include <glad/glad.h>

#include "common/primitives.hpp"
#include "graphics/opengl/util.hpp"
#include "graphics/vertex.hpp"

namespace gravel::gl {

Quad::Quad(const Vec3& bl, const Vec3& br, const Vec3& tr, const Vec3& tl)
{
  mVAO.bind();

  const Vertex vertices[] = {
      {
          .position = bl,
          .normal = Vec3 {0, 1, 0},
          .tex_coords = Vec2 {0, 0},
      },
      {
          .position = br,
          .normal = Vec3 {0, 1, 0},
          .tex_coords = Vec2 {1, 0},
      },
      {
          .position = tr,
          .normal = Vec3 {0, 1, 0},
          .tex_coords = Vec2 {1, 1},
      },
      {
          .position = tl,
          .normal = Vec3 {0, 1, 0},
          .tex_coords = Vec2 {0, 1},
      },
  };
  const uint indices[] = {0, 1, 2, 2, 3, 0};

  mVBO.bind();
  mVBO.upload_data(sizeof vertices, vertices);

  mVAO.init_attr(0, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, position));
  mVAO.init_attr(1, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, normal));
  mVAO.init_attr(2, 2, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, tex_coords));

  mEBO.bind();
  mEBO.upload_data(sizeof indices, indices);

  VertexArray::unbind();
  VertexBuffer::unbind();
  IndexBuffer::unbind();
}

void Quad::draw()
{
  mVAO.bind();

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
  GRAVEL_GL_CHECK_ERRORS();

  VertexArray::unbind();
}

void Quad::draw_without_depth_test()
{
  GLboolean depth_was_enabled = GL_FALSE;
  glGetBooleanv(GL_DEPTH_TEST, &depth_was_enabled);

  glDisable(GL_DEPTH_TEST);
  draw();

  if (depth_was_enabled == GL_TRUE) {
    glEnable(GL_DEPTH_TEST);
  }
}

}  // namespace gravel::gl
