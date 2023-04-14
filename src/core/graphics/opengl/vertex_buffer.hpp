#pragma once

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "graphics/opengl/buffer_usage.hpp"

namespace glow::gl {

/// Represents an OpenGL vertex buffer object (VBO).
class VertexBuffer final {
 public:
  GRAVEL_DELETE_COPY(VertexBuffer);

  /// Creates a VBO instance, but does not bind it.
  VertexBuffer();

  ~VertexBuffer() noexcept;

  VertexBuffer(VertexBuffer&& other) noexcept;

  auto operator=(VertexBuffer&& other) noexcept -> VertexBuffer&;

  /// Binds the VBO for subsequent draw calls.
  void bind() const;

  /// Unbinds any bound VBO.
  static void unbind();

  /**
   * Uploads data to the VBO.
   *
   * \pre The VBO must be bound when this function is called.
   *
   * \param data_size the size of the buffer, in bytes.
   * \param data pointer to the data that will be copied into the VBO.
   * \param usage buffer usage hint.
   */
  void upload_data(usize data_size,
                   const void* data,
                   BufferUsage usage = BufferUsage::Static);

  /// Returns the OpenGL identifier associated with the VBO.
  [[nodiscard]] auto get_id() const -> uint { return mID; }

 private:
  uint mID {};

  void dispose() noexcept;
};

}  // namespace glow::gl
