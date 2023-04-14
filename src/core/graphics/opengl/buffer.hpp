#pragma once

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "graphics/opengl/buffer_usage.hpp"

namespace glow::gl {

class Buffer final {
 public:
  GRAVEL_DELETE_COPY(Buffer);

  explicit Buffer(uint type);

  ~Buffer() noexcept;

  Buffer(Buffer&& other) noexcept;

  auto operator=(Buffer&& other) noexcept -> Buffer&;

  void bind() const;
  void unbind() const;

  void bind_block(int binding);
  void unbind_block(int binding);

  void reserve(ssize data_size, BufferUsage usage);

  void set_data(const void* data, ssize data_size, BufferUsage usage);

  void set_subdata(ssize offset, const void* data, ssize data_size);

  [[nodiscard]] auto is_vertex_buffer() const -> bool;
  [[nodiscard]] auto is_index_buffer() const -> bool;
  [[nodiscard]] auto is_uniform_buffer() const -> bool;

  [[nodiscard]] auto get_id() const -> uint { return mID; }

 private:
  uint mType {};
  uint mID {};

  void dispose() noexcept;
};

}  // namespace glow::gl
