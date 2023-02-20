#include "error.hpp"

#include <utility>  // move

namespace gravel {

Error::Error(String msg)
    : mMsg {std::move(msg)}
{
}

auto Error::what() const noexcept -> const char*
{
  return mMsg.c_str();
}

}  // namespace gravel