#pragma once

#include "common/primitives.hpp"
#include "graphics/opengl/index_buffer.hpp"
#include "graphics/opengl/vertex_array.hpp"
#include "graphics/opengl/vertex_buffer.hpp"

namespace gravel::comp {

struct OpenGLModel final {
  gl::VertexArray vao;
  gl::VertexBuffer vbo;
  gl::IndexBuffer ebo;
  usize index_count {};
};

}  // namespace gravel::comp