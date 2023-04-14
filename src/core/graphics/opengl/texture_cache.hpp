#pragma once

#include "common/predef.hpp"
#include "common/type/map.hpp"
#include "common/type/path.hpp"
#include "graphics/opengl/texture_2d.hpp"

namespace glow::gl {

/// Context component used to keep track of loaded textures.
struct TextureCache final {
  TextureCache() = default;
  ~TextureCache() = default;
  GRAVEL_DELETE_COPY(TextureCache);
  GRAVEL_DEFAULT_MOVE(TextureCache);

  Map<Path, Texture2D> textures;
};

}  // namespace glow::gl
