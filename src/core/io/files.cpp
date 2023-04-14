#include "files.hpp"

#include <ios>       // ios
#include <iterator>  // istreambuf_iterator

#include <SDL2/SDL.h>
#include <fmt/format.h>

#include "common/debug/error.hpp"
#include "common/type/memory.hpp"

namespace glow {
namespace {

struct SDLPathDeleter final {
  void operator()(char* path) noexcept { SDL_free(path); }
};

[[nodiscard]] auto determine_persistent_file_dir() -> Path
{
  Unique<char, SDLPathDeleter> raw_path {SDL_GetPrefPath("albin-johansson", "glow")};

  if (!raw_path) {
    throw Error {fmt::format("[IO] Could not determine persistent file directory: {}",
                             SDL_GetError())};
  }

  return Path {raw_path.get()};
}

}  // namespace

auto open_input_stream(const Path& file, const FileType type) -> Maybe<IfStream>
{
  const auto flags = (type == FileType::Binary) ? std::ios::in | std::ios::binary  //
                                                : std::ios::in;
  IfStream stream {file, flags};
  if (stream.is_open() && stream.good()) {
    return stream;
  }

  return kNothing;
}

auto load_file_as_string(const Path& file) -> Maybe<String>
{
  if (auto stream = open_input_stream(file)) {
    String content {std::istreambuf_iterator {*stream},
                    std::istreambuf_iterator<char> {}};
    return content;
  }

  return kNothing;
}

auto load_binary_file(const Path& path) -> Maybe<Vector<char>>
{
  if (auto stream = open_input_stream(path, FileType::Binary)) {
    Vector<char> content {std::istreambuf_iterator {*stream},
                          std::istreambuf_iterator<char> {}};
    return content;
  }

  return kNothing;
}

auto create_file(const Path& path, FileType type) -> Maybe<OfStream>
{
  auto flags = std::ios::out | std::ios::trunc;
  if (type == FileType::Binary) {
    flags |= std::ios::binary;
  }

  OfStream stream {path, flags};
  if (stream.is_open() && stream.good()) {
    return stream;
  }

  return kNothing;
}

auto get_persistent_file_dir() -> const Path&
{
  static const auto dir = determine_persistent_file_dir();
  return dir;
}

}  // namespace glow