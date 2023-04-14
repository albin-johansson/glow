#include "buffer.hpp"

#include <glad/glad.h>

#include "common/debug/assert.hpp"
#include "common/debug/error.hpp"
#include "common/type/vector.hpp"
#include "graphics/opengl/util.hpp"

namespace glow::gl {

Buffer::Buffer(const uint type)
    : mType {type}
{
  if (mType != GL_ARRAY_BUFFER && mType != GL_ELEMENT_ARRAY_BUFFER &&
      mType != GL_UNIFORM_BUFFER) {
    throw Error {"[GL] Invalid buffer type"};
  }

  glGenBuffers(1, &mID);
}

Buffer::~Buffer() noexcept
{
  dispose();
}

void Buffer::dispose() noexcept
{
  if (mID != 0) {
    glDeleteBuffers(1, &mID);
  }
}

Buffer::Buffer(Buffer&& other) noexcept
    : mType {other.mType},
      mID {other.mID}
{
  other.mID = 0;
}

auto Buffer::operator=(Buffer&& other) noexcept -> Buffer&
{
  GRAVEL_ASSERT(mType == other.mType);

  if (this != &other) {
    dispose();

    mID = other.mID;
    mType = other.mType;

    other.mID = 0;
  }

  return *this;
}

void Buffer::bind() const
{
  glBindBuffer(mType, mID);
  GRAVEL_GL_CHECK_ERRORS();
}

void Buffer::unbind() const
{
  glBindBuffer(mType, 0);
  GRAVEL_GL_CHECK_ERRORS();
}

void Buffer::bind_block(const int binding)
{
  GRAVEL_ASSERT(mType == GL_UNIFORM_BUFFER);

  glBindBufferBase(mType, binding, mID);
  GRAVEL_GL_CHECK_ERRORS();
}

void Buffer::unbind_block(const int binding)
{
  GRAVEL_ASSERT(mType == GL_UNIFORM_BUFFER);

  glBindBufferBase(mType, binding, 0);
  GRAVEL_GL_CHECK_ERRORS();
}

void Buffer::reserve(const ssize data_size, const BufferUsage usage)
{
  Vector<Byte> buffer;
  buffer.resize(static_cast<usize>(data_size));

  set_data(buffer.data(), std::ssize(buffer), usage);
}

void Buffer::set_data(const void* data, const ssize data_size, const BufferUsage usage)
{
  glBufferData(mType,
               static_cast<GLsizeiptr>(data_size),
               data,
               convert_buffer_usage(usage));
  GRAVEL_GL_CHECK_ERRORS();
}

void Buffer::set_subdata(const ssize offset, const void* data, const ssize data_size)
{
  glBufferSubData(mType, offset, data_size, data);
  GRAVEL_GL_CHECK_ERRORS();
}

auto Buffer::is_vertex_buffer() const -> bool
{
  return mType == GL_ARRAY_BUFFER;
}

auto Buffer::is_index_buffer() const -> bool
{
  return mType == GL_ELEMENT_ARRAY_BUFFER;
}

auto Buffer::is_uniform_buffer() const -> bool
{
  return mType == GL_UNIFORM_BUFFER;
}

}  // namespace glow::gl
