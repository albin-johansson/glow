#pragma once

namespace gravel {

/// Context component for input-related camera settings.
struct CameraOptions final {
  float speed {10};          ///< Movement speed factor.
  float sensitivity {0.5f};  ///< View sensitivity factor.
};

}  // namespace gravel
