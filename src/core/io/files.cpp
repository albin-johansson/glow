#include "files.hpp"

#include <ios>       // ios
#include <iterator>  // istreambuf_iterator

namespace gravel {

auto open_input_stream(const Path& file, const FileType type) -> Maybe<IfStream>
{
  const auto flags = (type == FileType::Binary) ? std::ios::in | std::ios::binary  //
                                                : std::ios::in;
  IfStream stream {file, flags};
  if (stream.is_open() && stream.good()) {
    return stream;
  }
  else {
    return kNothing;
  }
}

auto load_file_as_string(const Path& file) -> Maybe<String>
{
  if (auto stream = open_input_stream(file)) {
    String content {std::istreambuf_iterator {*stream},
                    std::istreambuf_iterator<char> {}};
    return content;
  }
  else {
    return kNothing;
  }
}

}  // namespace gravel