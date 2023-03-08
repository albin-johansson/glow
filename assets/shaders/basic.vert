#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex_coords;

out VsOutput {
  vec3 ws_position;
  vec3 vs_position;
  vec3 vs_normal;
  vec2 tex_coords;
} Out;

// Binding 0
layout (std140) uniform DynamicMatrices {
  mat4 m;
  mat4 mv;
  mat4 mvp;
  mat4 normal;
} DM;

void main()
{
  gl_Position = DM.mvp * vec4(position, 1);

  Out.ws_position = vec3(DM.m * vec4(position, 1));
  Out.vs_position = vec3(DM.mv * vec4(position, 1));
  Out.vs_normal = vec3(DM.normal * vec4(normal, 0));
  Out.tex_coords = tex_coords;
}