#pragma once

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "graphics/opengl/buffer_usage.hpp"

namespace glow::gl {

/// Represents an OpenGL element (or index) buffer object (EBO).
class IndexBuffer final {
 public:
  GRAVEL_DELETE_COPY(IndexBuffer);

  /// Creates an index buffer, but does not bind it.
  IndexBuffer();

  ~IndexBuffer() noexcept;

  IndexBuffer(IndexBuffer&& other) noexcept;

  auto operator=(IndexBuffer&& other) noexcept -> IndexBuffer&;

  /// Binds the index buffer for subsequent draw calls.
  void bind() const;

  /// Unbinds any bound index buffer.
  static void unbind();

  /**
   * Uploads indices to the index buffer.
   *
   * \pre The EBO must be bound when this function is called.
   *
   * \param data_size the size of the data, in bytes.
   * \param data the raw index data.
   * \param usage buffer usage optimization hint.
   */
  void upload_data(usize data_size,
                   const uint* data,
                   BufferUsage usage = BufferUsage::Static);

  [[nodiscard]] auto get_id() const -> uint { return mID; }

 private:
  uint mID {};

  void dispose() noexcept;
};

}  // namespace glow::gl
