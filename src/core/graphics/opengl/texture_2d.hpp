#pragma once

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/math.hpp"

namespace gravel::gl {

/// Represents a two-dimensional OpenGL texture.
class Texture2D final {
 public:
  GRAVEL_DELETE_COPY(Texture2D);

  /**
   * Creates a 2D texture.
   *
   * \details The texture will use linear minification and magnification by default, along
   *          with a clamp-to-edge strategy for wrapping. The texture will not be bound
   *          when the constructor returns.
   */
  Texture2D();

  ~Texture2D() noexcept;

  Texture2D(Texture2D&& other) noexcept;

  auto operator=(Texture2D&& other) noexcept -> Texture2D&;

  /// Enables the texture for subsequent draw calls.
  void bind();

  /// Enables a texture for subsequent draw calls.
  static void bind(uint id);

  /// Unbinds any bound 2D texture.
  static void unbind();

  /**
   * Sets the pixel data associated with texture.
   *
   * \pre The texture must be bound when this function is called.
   *
   * \param detail_level the level-of-detail (LOD) index.
   * \param texture_format  the texture format, e.g. 'GL_RGB'.
   * \param pixel_format the format of the pixel data, e.g. 'GL_RGB'.
   * \param type the texture format data type, e.g. 'GL_UNSIGNED_BYTE'.
   * \param size the texture size.
   * \param pixel_data the raw pixel data.
   */
  void set_data(int detail_level,
                int texture_format,
                uint pixel_format,
                uint type,
                const Vec2i& size,
                const void* pixel_data);

  [[nodiscard]] auto get_id() const -> uint { return mID; }

 private:
  uint mID {};

  void dispose() noexcept;
};

}  // namespace gravel::gl
