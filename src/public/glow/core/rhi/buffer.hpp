#pragma once

#include "glow/core/foundation/flags.hpp"
#include "glow/core/prelude.hpp"

namespace glow {

enum class BufferUsage : uint32 {
  None = 0,
  TransferSrc = 1u << 1u,
  TransferDst = 1u << 2u,
  VertexBuffer = 1u << 3u,
  IndexBuffer = 1u << 4u,
  UniformBuffer = 1u << 5u,
};

GLOW_FLAG_ENUM(BufferUsage)

class IBuffer {
 public:
  virtual ~IBuffer() noexcept = default;

  [[nodiscard]] virtual auto get_usage() const -> BufferUsage = 0;
};

}  // namespace glow