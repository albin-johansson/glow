#pragma once

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/math.hpp"
#include "common/type/maybe.hpp"
#include "common/type/path.hpp"

namespace glow::gl {

/// Represents a two-dimensional OpenGL texture.
class Texture2D final {
 public:
  GLOW_DELETE_COPY(Texture2D);

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

  [[nodiscard]] static auto load_rgb(const Path& path) -> Maybe<Texture2D>;

  [[nodiscard]] static auto load_rgb_f32(const Path& path) -> Maybe<Texture2D>;

  /// Enables the texture for subsequent draw calls.
  void bind() const;

  /// Enables a texture for subsequent draw calls.
  static void bind(uint id);

  /// Unbinds any bound 2D texture.
  static void unbind();

  /// Sets the pixel data associated with texture.
  ///
  /// \pre The texture must be bound when this function is called.
  ///
  /// \param detail_level the level-of-detail (LOD) index.
  /// \param texture_format  the texture format, e.g. 'GL_RGB'.
  /// \param pixel_format the format of the pixel data, e.g. 'GL_RGB'.
  /// \param type the texture format data type, e.g. 'GL_UNSIGNED_BYTE'.
  /// \param size the texture size.
  /// \param pixel_data the raw pixel data.
  void set_data(int detail_level,
                int texture_format,
                uint pixel_format,
                uint type,
                const Vec2i& size,
                const void* pixel_data);

  /// Generates a mipmap for the texture.
  ///
  /// \note This should be called after the texture data has been provided.
  ///
  /// \pre The texture must be bound when this function is called.
  void generate_mipmap();

  /// Specifies whether anisotropic filtering should be used by the texture.
  ///
  /// \pre The texture must be bound when this function is called.
  void set_anisotropic_filtering(bool enable);

  [[nodiscard]] auto get_id() const -> uint { return mID; }

 private:
  uint mID {};

  void dispose() noexcept;
};

}  // namespace glow::gl
