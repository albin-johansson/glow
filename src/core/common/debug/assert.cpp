#include "assert.hpp"

#include <exception>  // terminate

#include <spdlog/spdlog.h>

namespace boost {

void assertion_failed_msg(const char* expr,
                          const char* msg,
                          const char* /* function */,
                          const char* file,
                          const long line)
{
  spdlog::critical("{}:{} expression '{}' evaluated to false: {}",
                   file,
                   line,
                   expr,
                   msg ? msg : "N/A");
  std::terminate();
}

}  // namespace boost
