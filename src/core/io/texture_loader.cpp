#include "texture_loader.hpp"

#define STB_IMAGE_IMPLEMENTATION

#include <fmt/std.h>
#include <spdlog/spdlog.h>
#include <stb_image.h>

#include "common/debug/error.hpp"

namespace glow {
namespace {

[[nodiscard]] auto _convert_texture_channels(const TextureChannels format) -> int
{
  switch (format) {
    case TextureChannels::Gray:
      return STBI_grey;

    case TextureChannels::GrayAlpha:
      return STBI_grey_alpha;

    case TextureChannels::RGB:
      return STBI_rgb;

    case TextureChannels::RGBA:
      return STBI_rgb_alpha;

    default:
      throw Error {"Invalid texture channels"};
  }
}

}  // namespace

void PixelDataDeleter::operator()(uint8* data) noexcept
{
  stbi_image_free(data);
}

auto load_texture_data(const Path& path,
                       const TextureFormat format,
                       const TextureChannels channels) -> Maybe<TextureData>
{
  const auto path_str = path.string();
  spdlog::info("[IO] Loading texture {}", path);

  stbi_set_flip_vertically_on_load(true);

  TextureData data;
  void* pixels = nullptr;

  if (format == TextureFormat::Float) {
    pixels = stbi_loadf(path_str.c_str(),
                        &data.size.x,
                        &data.size.y,
                        &data.channels,
                        _convert_texture_channels(channels));
  }
  else {
    pixels = stbi_load(path_str.c_str(),
                       &data.size.x,
                       &data.size.y,
                       &data.channels,
                       convert_texture_channels(channels));
  }

  data.pixels.reset(static_cast<uint8*>(pixels));

  if (data.pixels) {
    return data;
  }
  else {
    spdlog::error("[IO] Failed to load texture: {}", stbi_failure_reason());
    return kNothing;
  }
}

}  // namespace glow