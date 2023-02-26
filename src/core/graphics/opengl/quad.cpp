#include "quad.hpp"

#include <glad/glad.h>

#include "common/primitives.hpp"
#include "graphics/opengl/util.hpp"
#include "graphics/vertex.hpp"

namespace gravel::gl {

Quad::Quad(const Vec2& bl, const Vec2& br, const Vec2& tr, const Vec2& tl)
{
  mVAO.bind();

  const Vec2 vertices[] = {bl, br, tr, tl};
  const uint indices[] = {0, 1, 2, 2, 3, 0};

  mVBO.bind();
  mVBO.upload_data(sizeof vertices, vertices);

  mVAO.init_attr(0, 2, GL_FLOAT);

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
