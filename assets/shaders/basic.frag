#version 410 core

precision highp float;

in VsOutput {
  vec3 ws_position;
  vec3 vs_position;
  vec3 vs_normal;
  vec2 tex_coords;
} In;

layout (location = 0) out vec4 frag_color;

void main()
{
  frag_color = vec4(normalize(In.vs_normal), 1.0);
}
