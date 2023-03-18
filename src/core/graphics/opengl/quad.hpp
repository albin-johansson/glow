#pragma once

#include "common/type/math.hpp"
#include "graphics/opengl/index_buffer.hpp"
#include "graphics/opengl/vertex_array.hpp"
#include "graphics/opengl/vertex_buffer.hpp"

namespace gravel::gl {

/// Utility class for rendering quads (rectangles).
class Quad final {
 public:
  /**
   * Creates a quad object.
   *
   * \param bl bottom-left vertex.
   * \param br bottom-right vertex.
   * \param tr upper-right vertex.
   * \param tl upper-left vertex.
   */
  Quad(const Vec3& bl, const Vec3& br, const Vec3& tr, const Vec3& tl);

  /// Creates a quad covering the entire screen.
  Quad()
      : Quad {{-1.0f, -1.0f, 0.0f}, {1.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {-1.0f, 1.0f, 0.0f}}
  {
  }

  void draw();

  void draw_without_depth_test();

 private:
  VertexArray mVAO;
  VertexBuffer mVBO;
  IndexBuffer mEBO;
};

}  // namespace gravel::gl
