#pragma once

namespace gravel {

/// Represents buffer usage optimization hints.
enum class BufferUsage {
  Static,   ///< Modified once, used many times.
  Dynamic,  ///< Modified repeatedly, used many times.
  Stream    ///< Modified once, used at most a few times.
};

}  // namespace gravel