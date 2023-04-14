#pragma once

#include "common/primitives.hpp"

namespace glow {

/// Context component for environment backdrop.
struct EnvironmentOptions final {
  float32 brightness {1.0f};
  float32 gamma {2.2f};
  bool use_gamma_correction {true};
};

}  // namespace glow
