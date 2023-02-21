#pragma once

#include "common/type/fstream.hpp"
#include "common/type/maybe.hpp"
#include "common/type/path.hpp"
#include "common/type/string.hpp"

namespace gravel {

enum class FileType {
  Text,
  Binary
};

[[nodiscard]] auto open_input_stream(const Path& file, FileType type = FileType::Text)
    -> Maybe<IfStream>;

[[nodiscard]] auto load_file_as_string(const Path& file) -> Maybe<String>;

}  // namespace gravel
