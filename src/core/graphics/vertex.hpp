#pragma once

#include <cstddef>     // size_t
#include <functional>  // hash

#include <glm/gtx/hash.hpp>

#include "common/hash.hpp"
#include "common/type/math.hpp"

namespace gravel {

struct Vertex final {
  Vec3 position {};
  Vec3 normal {};
  Vec2 tex_coords {};

  [[nodiscard]] auto operator==(const Vertex&) const -> bool = default;
};

}  // namespace gravel

template <>
struct std::hash<gravel::Vertex> final {
  [[nodiscard]] auto operator()(const gravel::Vertex& vertex) const -> std::size_t
  {
    return gravel::hash_combine(vertex.position, vertex.normal, vertex.tex_coords);
  }
};