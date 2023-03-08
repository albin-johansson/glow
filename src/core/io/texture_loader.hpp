#pragma once

#include <memory>  // unique_ptr

#include "common/primitives.hpp"
#include "common/type/math.hpp"
#include "common/type/maybe.hpp"
#include "common/type/path.hpp"

namespace gravel {

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

using PixelData = std::unique_ptr<uchar, PixelDataDeleter>;

struct TextureData final {
  PixelData pixels;
  Vec2i size {};
  int channels {};
};

[[nodiscard]] auto load_texture_data(const Path& path,
                                     TextureFormat format,
                                     TextureChannels channels) -> Maybe<TextureData>;

}  // namespace gravel