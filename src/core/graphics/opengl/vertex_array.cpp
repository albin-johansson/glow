#include "vertex_array.hpp"

#include <glad/glad.h>

#include "common/debug/assert.hpp"
#include "graphics/opengl/util.hpp"
#include "util/bits.hpp"

namespace glow::gl {

VertexArray::VertexArray()
{
  glGenVertexArrays(1, &mID);
}

VertexArray::~VertexArray() noexcept
{
  dispose();
}

VertexArray::VertexArray(VertexArray&& other) noexcept
    : mID {other.mID}
{
  other.mID = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
{
  if (this != &other) {
    dispose();

    mID = other.mID;
    other.mID = 0;
  }

  return *this;
}

void VertexArray::dispose() noexcept
{
  if (mID != 0) {
    glDeleteVertexArrays(1, &mID);
  }
}

void VertexArray::bind() const
{
  glBindVertexArray(mID);
}

void VertexArray::unbind()
{
  glBindVertexArray(0);
}

void VertexArray::init_attr(const uint location,
                            const int value_count,
                            const uint value_type,
                            const usize vertex_size,
                            const usize offset,
                            const bool normalized)
{
  GRAVEL_ASSERT(get_bound_vertex_array() == mID);

  glVertexAttribPointer(location,
                        value_count,
                        value_type,
                        normalized ? GL_TRUE : GL_FALSE,
                        static_cast<int>(vertex_size),
                        bitcast<void*>(offset));
  glEnableVertexAttribArray(location);

  GRAVEL_GL_CHECK_ERRORS();
}

}  // namespace glow::gl