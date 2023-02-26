#version 410 core

layout (location = 0) in vec2 position;

out VsOut {
  vec2 tex_coords;
} Out;

void main()
{
  gl_Position = vec4(position, 0, 1);

  Out.tex_coords = (position + vec2(1, 1)) * 0.5;
}
