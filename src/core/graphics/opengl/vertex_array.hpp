#pragma once

#include "common/predef.hpp"
#include "common/primitives.hpp"

namespace gravel::gl {

/// Represents an OpenGL vertex array object (VAO).
class VertexArray final {
 public:
  GRAVEL_DELETE_COPY(VertexArray);

  /// Creates a VAO instance, but does not bind it.
  VertexArray();

  ~VertexArray() noexcept;

  VertexArray(VertexArray&& other) noexcept;

  auto operator=(VertexArray&& other) noexcept -> VertexArray&;

  /// Binds the VAO for subsequent draw calls.
  void bind();

  /// Unbinds any bound VAO.
  static void unbind();

  /**
   * Initializes a vertex attribute slot and enables it.
   *
   * \pre The VAO must be bound when this function is called.
   *
   * \param location the vertex attribute index.
   * \param value_count the amount of values in the attribute, e.g. '3' for 3D vectors.
   * \param value_type the primitive type stored in the attribute, e.g. 'GL_FLOAT'.
   * \param vertex_size the total size of a vertex including all attributes.
   * \param offset the offset of the attribute.
   * \param normalized controls whether the values are normalized.
   */
  void init_attr(uint location,
                 int value_count,
                 uint value_type,
                 usize vertex_size = 0,
                 usize offset = 0,
                 bool normalized = false);

  /// Returns the OpenGL identifier associated with the VAO.
  [[nodiscard]] auto get_id() const -> uint { return mID; }

 private:
  uint mID {};

  void dispose() noexcept;
};

}  // namespace gravel::gl
