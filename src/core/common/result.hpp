#pragma once

#include "common/debug/error.hpp"

namespace glow {

/// Simple type that encodes to possible values: success or failure.
/// Intended to be used to communicate non-fatal errors in void functions.
class Result final {
 public:
  [[nodiscard]] constexpr static auto make_success() noexcept -> Result
  {
    return Result {true};
  }

  [[nodiscard]] constexpr static auto make_failure() noexcept -> Result
  {
    return Result {false};
  }

  [[nodiscard]] constexpr auto succeeded() const noexcept { return mSucceeded; }

  [[nodiscard]] constexpr auto failed() const noexcept { return !mSucceeded; }

  void check(const char* msg) const
  {
    if (!mSucceeded) {
      throw Error {msg ? msg : "?"};
    }
  }

 private:
  bool mSucceeded {};

  constexpr explicit Result(const bool success) noexcept
      : mSucceeded {success}
  {
  }
};

inline constexpr Result kSuccess = Result::make_success();
inline constexpr Result kFailure = Result::make_failure();

}  // namespace glow
