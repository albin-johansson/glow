#pragma once

namespace gravel {

/// Context component for input-related camera settings.
struct CameraOptions final {
  float speed {5};        ///< Movement speed factor.
  float sensitivity {1};  ///< View sensitivity factor.
};

}  // namespace gravel
