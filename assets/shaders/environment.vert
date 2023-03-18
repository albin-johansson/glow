#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex_coords;

out VsOut {
  vec2 tex_coords;
} Out;

void main()
{
  gl_Position = vec4(position, 1);

  Out.tex_coords = tex_coords;
}
