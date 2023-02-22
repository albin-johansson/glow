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
   * \param v0 bottom-left vertex.
   * \param v1 bottom-right vertex.
   * \param v2 upper-right vertex.
   * \param v3 upper-left vertex.
   */
  Quad(const Vec3& v0, const Vec3& v1, const Vec3& v2, const Vec3& v3);

  /// Creates a unit quad.
  Quad()
      : Quad {{-1.0f, -1.0f, 0.0f},
              {1.0f, -1.0f, 0.0f},
              {1.0f, 1.0f, 0.0f},
              {-1.0f, 1.0f, 0.0f}}
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
