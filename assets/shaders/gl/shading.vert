#version 410 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;

out VsOutput {
  vec3 ws_position;
  vec3 vs_position;
  vec3 vs_normal;
  vec2 tex_coords;
} Out;

layout (std140) uniform MatrixBuffer {
  mat4 uModelMatrix;
  mat4 uModelViewMatrix;
  mat4 uMVP;
  mat4 uNormalMatrix;
};

void main()
{
  gl_Position = uMVP * vec4(vPosition, 1);

  Out.ws_position = vec3(uModelMatrix * vec4(vPosition, 1));
  Out.vs_position = vec3(uModelViewMatrix * vec4(vPosition, 1));
  Out.vs_normal = vec3(uNormalMatrix * vec4(vNormal, 0));
  Out.tex_coords = vTexCoords;
}