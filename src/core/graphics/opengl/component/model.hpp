#pragma once

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/ecs.hpp"
#include "common/type/maybe.hpp"
#include "common/type/vector.hpp"
#include "graphics/opengl/index_buffer.hpp"
#include "graphics/opengl/texture_2d.hpp"
#include "graphics/opengl/vertex_array.hpp"
#include "graphics/opengl/vertex_buffer.hpp"

namespace gravel::comp {

struct OpenGLMaterial final {
  Maybe<gl::Texture2D> diffuse_tex;
  Maybe<gl::Texture2D> specular_tex;
};

struct OpenGLMesh final {
  gl::VertexArray vao;
  gl::VertexBuffer vbo;
  gl::IndexBuffer ebo;
  Entity material {kNullEntity};

  uint vertex_count {};
  uint index_count {};
};

struct OpenGLModel final {
  OpenGLModel() = default;
  ~OpenGLModel() = default;
  GRAVEL_DEFAULT_MOVE(OpenGLModel);
  GRAVEL_DELETE_COPY(OpenGLModel);

  Vector<OpenGLMesh> meshes;
};

}  // namespace gravel::comp