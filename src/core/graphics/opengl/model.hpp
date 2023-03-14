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

namespace gravel::gl {

/// OpenGL material component.
struct Material final {
  Maybe<uint> diffuse_tex;   ///< Optional ID of diffuse texture.
  Maybe<uint> specular_tex;  ///< Optional ID of specular texture.
  Vec3 ambient {};
  Vec3 diffuse {};
  Vec3 specular {};
  Vec3 emission {};
};

/// OpenGL mesh component.
struct Mesh final {
  Mat4 transform {1.0f};          ///< Transform matrix relative to parent model.
  VertexArray vao;                ///< Associated vertex array object.
  VertexBuffer vbo;               ///< Associated vertex buffer object.
  IndexBuffer ebo;                ///< Associated index buffer object.
  Entity material {kNullEntity};  ///< The associated material entity.
  uint index_count {};            ///< The amount of indices needed to render the mesh.
};

/// OpenGL model component.
struct Model final {
  Model() = default;
  ~Model() = default;
  GRAVEL_DEFAULT_MOVE(Model);
  GRAVEL_DELETE_COPY(Model);

  Vector<Mesh> meshes;  ///< The meshes that constitute the model.
};

/// Loads and assigns an OpenGL model component to an entity.
void assign_model(Registry& registry, Entity entity, const Path& path);

}  // namespace gravel::gl