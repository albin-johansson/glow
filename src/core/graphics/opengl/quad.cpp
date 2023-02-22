#include "quad.hpp"

#include <cstddef>  // offsetof

#include <glad/glad.h>

#include "common/primitives.hpp"
#include "graphics/opengl/util.hpp"
#include "graphics/vertex.hpp"

namespace gravel::gl {

Quad::Quad(const Vec3& v0, const Vec3& v1, const Vec3& v2, const Vec3& v3)
{
  mVAO.bind();

  const Vertex vertices[] = {
      Vertex {
          .position = v0,
          .normal = {0, 0, 1},
          .tex_coords = {0.0f, 0.0f},
      },
      Vertex {
          .position = v1,
          .normal = {0, 0, 1},
          .tex_coords = {1.0f, 0.0f},
      },
      Vertex {
          .position = v2,
          .normal = {0, 0, 1},
          .tex_coords = {1.0f, 1.0f},
      },
      Vertex {
          .position = v3,
          .normal = {0, 0, 1},
          .tex_coords = {0.0f, 1.0f},
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
