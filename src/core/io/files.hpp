#pragma once

#include "common/type/fstream.hpp"
#include "common/type/maybe.hpp"
#include "common/type/path.hpp"
#include "common/type/string.hpp"
#include "common/type/vector.hpp"

namespace gravel {

enum class FileType {
  Text,
  Binary
};

[[nodiscard]] auto open_input_stream(const Path& file, FileType type = FileType::Text)
    -> Maybe<IfStream>;

[[nodiscard]] auto load_file_as_string(const Path& file) -> Maybe<String>;

[[nodiscard]] auto load_binary_file(const Path& path) -> Maybe<Vector<char>>;

[[nodiscard]] auto create_file(const Path& path, FileType type = FileType::Text)
    -> Maybe<OfStream>;

[[nodiscard]] auto get_persistent_file_dir() -> const Path&;

}  // namespace gravel
