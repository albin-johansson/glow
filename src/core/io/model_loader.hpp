#pragma once

#include "common/type/maybe.hpp"
#include "common/type/path.hpp"
#include "common/type/string.hpp"
#include "common/type/vector.hpp"
#include "graphics/vertex.hpp"

namespace gravel {

struct ModelData final {
  Vector<Vertex> vertices;
  Vector<uint> indices;
};

[[nodiscard]] auto load_obj_model(const Path& path) -> Maybe<ModelData>;

}  // namespace gravel
