#include "vertex_buffer.hpp"

#include <cassert>  // assert

#include <glad/glad.h>

#include "graphics/opengl/util.hpp"

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

void VertexBuffer::upload_data(const usize data_size,
                               const void* data,
                               const BufferUsage usage)
{
  assert(get_bound_vertex_buffer() == mID);

  glBufferData(GL_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(data_size),
               data,
               convert_buffer_usage(usage));
  GRAVEL_GL_CHECK_ERRORS();
}

}  // namespace gravel::gl