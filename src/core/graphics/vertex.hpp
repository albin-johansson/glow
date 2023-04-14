#pragma once

#include <cstddef>     // size_t
#include <functional>  // hash

#include <glm/gtx/hash.hpp>

#include "common/hash.hpp"
#include "common/type/math.hpp"

namespace glow {

struct Vertex final {
  Vec3 position {};
  Vec3 normal {};
  Vec2 tex_coords {};

  [[nodiscard]] auto operator==(const Vertex&) const -> bool = default;
};

}  // namespace glow

template <>
struct std::hash<glow::Vertex> final {
  [[nodiscard]] auto operator()(const glow::Vertex& vertex) const -> std::size_t
  {
    return glow::hash_combine(vertex.position, vertex.normal, vertex.tex_coords);
  }
};