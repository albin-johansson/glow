#pragma once

#include <boost/assert.hpp>

#define GRAVEL_ASSERT(Expr) BOOST_ASSERT(Expr)
#define GRAVEL_ASSERT_MSG(Expr, Msg) BOOST_ASSERT_MSG(Expr, Msg)

namespace boost {

void assertion_failed_msg(const char* expr,
                          const char* msg,
                          const char* function,
                          const char* file,
                          long line);

inline void assertion_failed(const char* expr,
                             const char* function,
                             const char* file,
                             const long line)
{
  assertion_failed_msg(expr, nullptr, function, file, line);
}

}  // namespace boost
