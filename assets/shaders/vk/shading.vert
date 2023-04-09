#version 450 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;

layout (location = 0) out VsOutput {
  vec3 ws_position;
  vec3 vs_position;
  vec3 vs_normal;
  vec2 tex_coords;
} Out;

layout (std140, set = 0, binding = 0) uniform StaticMatrices {
  mat4 uProjMatrix;
  mat4 uViewMatrix;
  mat4 uViewProjMatrix;
};

layout (std140, push_constant) uniform PushConstants {
  mat4 uModelMatrix;
};

void main()
{
  mat4 mv_matrix = uViewMatrix * uModelMatrix;
  mat4 mvp_matrix = uProjMatrix * mv_matrix;
  mat4 normal_matrix = inverse(transpose(mv_matrix));

  gl_Position = mvp_matrix * vec4(vPosition, 1);

  Out.ws_position = vec3(uModelMatrix * vec4(vPosition, 1));
  Out.vs_position = vec3(mv_matrix * vec4(vPosition, 1));
  Out.vs_normal = vec3(normal_matrix * vec4(vNormal, 0));
  Out.tex_coords = vTexCoords;
}