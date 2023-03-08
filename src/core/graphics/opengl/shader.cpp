#include "shader.hpp"

#include <fmt/std.h>
#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include "graphics/opengl/util.hpp"
#include "io/files.hpp"

namespace gravel::gl {

Shader::Shader(const ShaderType type)
    : mID {glCreateShader(convert_shader_type(type))}
{
}

Shader::~Shader() noexcept
{
  dispose();
}

Shader::Shader(Shader&& other) noexcept
    : mID {other.mID}
{
  other.mID = 0;
}

auto Shader::operator=(Shader&& other) noexcept -> Shader&
{
  if (this != &other) {
    dispose();

    mID = other.mID;
    other.mID = 0;
  }

  return *this;
}

void Shader::dispose() noexcept
{
  if (mID != 0) {
    glDeleteShader(mID);
  }
}

auto Shader::compile_code(const char* code) -> Result
{
  glShaderSource(mID, 1, &code, nullptr);
  glCompileShader(mID);

  int success {};
  glGetShaderiv(mID, GL_COMPILE_STATUS, &success);

  if (success) {
    return kSuccess;
  }
  else {
    char msg[512] {};
    glGetShaderInfoLog(mID, sizeof msg, nullptr, msg);
    spdlog::error("[GL] Compilation error: {}", msg);
    return kFailure;
  }
}

auto Shader::compile_file(const Path& path) -> Result
{
  if (const auto source_code = load_file_as_string(path)) {
    return compile_code(source_code->c_str());
  }
  else {
    spdlog::error("[GL] Failed to read shader source file: {}", path);
    return kFailure;
  }
}

}  // namespace gravel::gl