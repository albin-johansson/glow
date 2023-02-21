#include "vertex_buffer.hpp"

#include <glad/glad.h>

namespace gravel::gl {

VertexBuffer::VertexBuffer()
{
  glGenBuffers(1, &mID);
}

VertexBuffer::~VertexBuffer() noexcept
{
  dispose();
}

VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
    : mID {other.mID}
{
  other.mID = 0;
}

auto VertexBuffer::operator=(VertexBuffer&& other) noexcept -> VertexBuffer&
{
  if (this != &other) {
    dispose();

    mID = other.mID;
    other.mID = 0;
  }

  return *this;
}

void VertexBuffer::dispose() noexcept
{
  if (mID != 0) {
    glDeleteBuffers(1, &mID);
  }
}

void VertexBuffer::bind()
{
  glBindBuffer(GL_ARRAY_BUFFER, mID);
}

void VertexBuffer::unbind()
{
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

}  // namespace gravel::gl