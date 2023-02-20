#pragma once

#include <exception>  // exception

#include "common/type/string.hpp"

namespace gravel {

class Error final : public std::exception {
 public:
  explicit Error(String msg);

  [[nodiscard]] auto what() const noexcept -> const char* override;

 private:
  String mMsg;
};

}  // namespace gravel
