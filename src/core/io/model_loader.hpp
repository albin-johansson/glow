#pragma once

#include "common/primitives.hpp"
#include "common/type/maybe.hpp"
#include "common/type/path.hpp"
#include "common/type/string.hpp"
#include "common/type/vector.hpp"
#include "graphics/vertex.hpp"

namespace gravel {

struct MaterialData final {
  Maybe<Path> diffuse_tex;
  Maybe<Path> specular_tex;
};

struct MeshData final {
  Vector<Vertex> vertices;
  Vector<uint> indices;
};

struct ModelData final {
  Vector<MaterialData> materials;
  Vector<MeshData> meshes;
};

[[nodiscard]] auto load_obj_model(const Path& path) -> Maybe<ModelData>;

}  // namespace gravel
