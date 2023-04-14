#include "uniform_buffer.hpp"

#include <vector>  // vector

#include <glad/glad.h>

#include "common/debug/assert.hpp"
#include "graphics/opengl/util.hpp"

namespace glow::gl {

UniformBuffer::UniformBuffer()
{
  glGenBuffers(1, &mID);
}

UniformBuffer::~UniformBuffer() noexcept
{
  dispose();
}

UniformBuffer::UniformBuffer(UniformBuffer&& other) noexcept
    : mID {other.mID}
{
  other.mID = 0;
}

auto UniformBuffer::operator=(UniformBuffer&& other) noexcept -> UniformBuffer&
{
  if (this != &other) {
    dispose();

    mID = other.mID;
    other.mID = 0;
  }

  return *this;
}

void UniformBuffer::dispose() noexcept
{
  if (mID != 0) {
    glDeleteBuffers(1, &mID);
  }
}

void UniformBuffer::bind() const
{
  glBindBuffer(GL_UNIFORM_BUFFER, mID);
  GRAVEL_GL_CHECK_ERRORS();
}

void UniformBuffer::unbind()
{
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  GRAVEL_GL_CHECK_ERRORS();
}

void UniformBuffer::bind_block(const int binding)
{
  glBindBufferBase(GL_UNIFORM_BUFFER, binding, mID);
  GRAVEL_GL_CHECK_ERRORS();
}

void UniformBuffer::unbind_block(const int binding)
{
  glBindBufferBase(GL_UNIFORM_BUFFER, binding, 0);
  GRAVEL_GL_CHECK_ERRORS();
}

void UniformBuffer::reserve_space(const ssize data_size, const BufferUsage usage)
{
  GRAVEL_ASSERT(mID == get_bound_uniform_buffer());

  std::vector<Byte> buffer;
  buffer.resize(static_cast<usize>(data_size));

  set_data(std::ssize(buffer), buffer.data(), usage);
}

void UniformBuffer::set_data(const ssize data_size,
                             const void* data,
                             const BufferUsage usage)
{
  GRAVEL_ASSERT(mID == get_bound_uniform_buffer());

  glBufferData(GL_UNIFORM_BUFFER, data_size, data, convert_buffer_usage(usage));
  GRAVEL_GL_CHECK_ERRORS();
}

void UniformBuffer::update_data(const ssize offset,
                                const ssize data_size,
                                const void* data)
{
  GRAVEL_ASSERT(mID == get_bound_uniform_buffer());

  glBufferSubData(GL_UNIFORM_BUFFER, offset, data_size, data);
  GRAVEL_GL_CHECK_ERRORS();
}

}  // namespace glow::gl
