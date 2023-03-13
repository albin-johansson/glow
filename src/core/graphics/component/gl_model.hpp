#pragma once

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/ecs.hpp"
#include "common/type/math.hpp"
#include "common/type/maybe.hpp"
#include "common/type/path.hpp"
#include "common/type/vector.hpp"
#include "graphics/opengl/index_buffer.hpp"
#include "graphics/opengl/vertex_array.hpp"
#include "graphics/opengl/vertex_buffer.hpp"

namespace gravel {
namespace comp {

struct OpenGLMaterial final {
  Maybe<uint> diffuse_tex;
  Maybe<uint> specular_tex;

  Vec3 ambient {};
  Vec3 diffuse {};
  Vec3 specular {};
  Vec3 emission {};
};

struct OpenGLMesh final {
  Mat4 transform {1.0f};
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

}  // namespace comp

/// Loads and assigns an OpenGL model component to an entity.
void assign_opengl_model(Registry& registry, Entity entity, const Path& path);

}  // namespace gravel