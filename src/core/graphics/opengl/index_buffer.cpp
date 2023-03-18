#include "index_buffer.hpp"

#include <glad/glad.h>

#include "common/debug/assert.hpp"
#include "graphics/opengl/util.hpp"

namespace gravel::gl {

IndexBuffer::IndexBuffer()
{
  glGenBuffers(1, &mID);
}

IndexBuffer::~IndexBuffer() noexcept
{
  dispose();
}

IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept
    : mID {other.mID}
{
  other.mID = 0;
}

auto IndexBuffer::operator=(IndexBuffer&& other) noexcept -> IndexBuffer&
{
  if (this != &other) {
    dispose();

    mID = other.mID;
    other.mID = 0;
  }

  return *this;
}

void IndexBuffer::dispose() noexcept
{
  if (mID != 0) {
    glDeleteBuffers(1, &mID);
  }
}

void IndexBuffer::bind() const
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);
}

void IndexBuffer::unbind()
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::upload_data(const usize data_size,
                              const uint* data,
                              const BufferUsage usage)
{
  GRAVEL_ASSERT(get_bound_index_buffer() == mID);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(data_size),
               data,
               convert_buffer_usage(usage));
  GRAVEL_GL_CHECK_ERRORS();
}

}  // namespace gravel::gl