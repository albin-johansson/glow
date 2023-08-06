#pragma once

#include "glow/core/foundation/flags.hpp"
#include "glow/core/prelude.hpp"

namespace glow {

using RenderID = uint64;

struct RenderDeviceInfo;
struct RenderDeviceFeatures;
struct VertexAttribute;
struct VertexDeclaration;

struct PipelineRasterizerState;
struct PipelineDepthStencilState;
struct PipelineColorBlendState;

class IRenderContext;
class IRenderDevice;
class IResourceManager;
class IBuffer;
class ITexture;
class ICommandBuffer;
class IGraphicsPipeline;
class IShader;

enum class Handedness : uint32;
enum class DescriptorType : uint32;
enum class IndexType : uint32;
enum class PolygonMode : uint32;
enum class CullMode : uint32;
enum class PixelFormat : uint32;
enum class ImageLayout : uint32;

GLOW_FWD_FLAG_ENUM(ShaderStage, uint32);
GLOW_FWD_FLAG_ENUM(MemoryHint, uint32);
GLOW_FWD_FLAG_ENUM(BufferHint, uint32);
GLOW_FWD_FLAG_ENUM(BufferUsage, uint32);

}  // namespace glow
