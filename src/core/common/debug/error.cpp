#include "error.hpp"

#include <sstream>  // stringstream
#include <utility>  // move

#include <boost/stacktrace/stacktrace.hpp>

namespace gravel {

namespace bst = boost::stacktrace;

Error::Error(String msg)
    : mMsg {std::move(msg)}
{
  std::stringstream stream;
  stream << bst::stacktrace {};
  mTrace = stream.str();
}

auto Error::what() const noexcept -> const char*
{
  return mMsg.c_str();
}

auto Error::trace() const -> const String&
{
  return mTrace;
}

}  // namespace gravel