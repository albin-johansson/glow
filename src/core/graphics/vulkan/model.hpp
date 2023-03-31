#pragma once

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/ecs.hpp"
#include "common/type/math.hpp"
#include "common/type/maybe.hpp"
#include "common/type/path.hpp"
#include "common/type/vector.hpp"
#include "graphics/vulkan/buffer.hpp"

namespace gravel {
GRAVEL_FORWARD_DECLARE_C(Scene);
}  // namespace gravel

namespace gravel::vlk {

/// Vulkan material component.
struct Material final {
  // TODO diffuse texture
  // TODO specular texture
  Vec3 ambient {};
  Vec3 diffuse {};
  Vec3 specular {};
  Vec3 emission {};
};

/// Vulkan mesh component.
/// TODO use one buffer for the vertex and index data
struct Mesh final {
  GRAVEL_MOVE_ONLY_COMPONENT(Mesh);

  Mat4 transform {1.0f};          ///< Transform matrix relative to parent model.
  Maybe<Buffer> vertex_buffer;    ///< Associated vertex buffer.
  Maybe<Buffer> index_buffer;     ///< Associated index buffer.
  Entity material {kNullEntity};  ///< The associated material entity.
  uint32 index_count {};          ///< The amount of indices needed to render the mesh.
};

/// Vulkan model component.
struct Model final {
  GRAVEL_MOVE_ONLY_COMPONENT(Model);

  Vector<Mesh> meshes;
};

/// Loads and assigns a Vulkan model component to an entity.
void assign_model(Scene& scene, Entity entity, const Path& path);

}  // namespace gravel::vlk
