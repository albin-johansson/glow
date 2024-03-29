#pragma once

#include <exception>  // exception

#include "common/predef.hpp"
#include "common/type/string.hpp"

namespace glow {

class Error final : public std::exception {
 public:
  GLOW_NOINLINE explicit Error(String msg);

  [[nodiscard]] auto what() const noexcept -> const char* override;

  [[nodiscard]] auto trace() const -> const String&;

 private:
  String mMsg;
  String mTrace;
};

/// Custom terminate handler.
[[noreturn]] GLOW_NOINLINE void on_terminate();

}  // namespace glow
