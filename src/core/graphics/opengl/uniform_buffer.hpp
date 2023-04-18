#pragma once

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "graphics/opengl/buffer_usage.hpp"

namespace glow::gl {

/// Represents an OpenGL uniform buffer object (UBO).
class UniformBuffer final {
 public:
  GLOW_DELETE_COPY(UniformBuffer);

  /// Creates a UBO, but does not bind it.
  UniformBuffer();

  ~UniformBuffer() noexcept;

  UniformBuffer(UniformBuffer&& other) noexcept;

  auto operator=(UniformBuffer&& other) noexcept -> UniformBuffer&;

  /// Binds the UBO for subsequent draw calls.
  void bind() const;

  /// Unbinds any bound UBO.
  static void unbind();

  /// Binds the UBO to the specified binding point.
  void bind_block(int binding);

  /// Unbinds any bound UBO from the specified binding point.
  static void unbind_block(int binding);

  /**
   * \brief Allocates the specified amount of zero-initialized storage in the UBO.
   *
   * \pre The UBO must be bound when this function is called.
   *
   * \param data_size the size of the storage, in bytes.
   * \param usage buffer usage hint.
   */
  void reserve_space(ssize data_size, BufferUsage usage = BufferUsage::Dynamic);

  /**
   * Replaces the data in the UBO.
   *
   * \pre The UBO must be bound when this function is called.
   *
   * \note Frequent updates to the buffer data should be done using `update_data`.
   *
   * \param data_size the size of the data, in bytes.
   * \param data pointer to the data that will be copied into the UBO.
   * \param usage buffer usage hint.
   */
  void set_data(ssize data_size,
                const void* data,
                BufferUsage usage = BufferUsage::Dynamic);

  /**
   * Updates existing data in the UBO.
   *
   * \pre The UBO must be bound when this function is called.
   *
   * \param offset the offset of the data member that will be changed.
   * \param data_size the size of the data member, in bytes.
   * \param data the new data.
   */
  void update_data(ssize offset, ssize data_size, const void* data);

 private:
  uint mID {};

  void dispose() noexcept;
};

}  // namespace glow::gl
