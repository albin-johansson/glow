#pragma once

#include "common/primitives.hpp"
#include "common/type/math.hpp"
#include "common/type/maybe.hpp"
#include "common/type/memory.hpp"
#include "common/type/path.hpp"

namespace glow {

enum class TextureFormat {
  Byte,
  Float
};

enum class TextureChannels {
  RGB,
  RGBA,
  Gray,
  GrayAlpha
};

struct PixelDataDeleter final {
  void operator()(uchar* data) noexcept;
};

using PixelData = Unique<uchar, PixelDataDeleter>;

struct TextureData final {
  PixelData pixels;
  Vec2i size {};
  int channels {};
};

[[nodiscard]] auto load_texture_data(const Path& path,
                                     TextureFormat format,
                                     TextureChannels channels) -> Maybe<TextureData>;

}  // namespace glow