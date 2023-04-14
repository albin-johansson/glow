#pragma once

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/math.hpp"
#include "graphics/opengl/texture_2d.hpp"

namespace glow::gl {

/// Represents an OpenGL framebuffer object, providing a color and depth buffer.
class Framebuffer final {
 public:
  GRAVEL_DELETE_COPY(Framebuffer);

  /**
   * Creates a framebuffer object (including texture objects), but does not bind it.
   *
   * \note You need to call the resize function to make the framebuffer usable
   */
  Framebuffer();

  ~Framebuffer() noexcept;

  Framebuffer(Framebuffer&& other) noexcept;

  auto operator=(Framebuffer&& other) noexcept -> Framebuffer&;

  /// Binds the framebuffer for subsequent draw calls.
  void bind() const;

  /// Unbinds any bound framebuffer.
  static void unbind();

  /**
   * Sets the size of the framebuffer.
   *
   * \pre The framebuffer must be bound when this function is called.
   * \param size the framebuffer size.
   */
  void resize(Vec2i size);

  /**
   * Sets the viewport and clears the color and depth buffers.
   *
   * \pre The framebuffer must be bound when this function is called.
   * \pre The framebuffer must be complete when this function is called.
   */
  void clear();

  [[nodiscard]] auto is_complete() const -> bool;

  [[nodiscard]] auto get_id() const -> uint;

  [[nodiscard]] auto get_color_texture_id() const -> uint;

  [[nodiscard]] auto get_depth_texture_id() const -> uint;

  [[nodiscard]] auto get_size() const -> const Vec2i&;

 private:
  uint mID {};
  Texture2D mColorTexture;
  Texture2D mDepthTexture;
  Vec2i mSize {};

  void dispose() noexcept;
};

}  // namespace glow::gl
