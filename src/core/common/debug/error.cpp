#include "error.hpp"

#include <cstdlib>  // abort
#include <sstream>  // stringstream
#include <utility>  // move

#include <boost/stacktrace/stacktrace.hpp>
#include <spdlog/spdlog.h>

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

void on_terminate()
{
  try {
    std::stringstream stream;
    stream << bst::stacktrace {};
    spdlog::critical("Into exile I must go. Failed I have.\n{}", stream.str());
  }
  catch (...) {
    // Not much we can do.
  }

  std::abort();
}

}  // namespace gravel