#version 410 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;

out VsOut {
  vec2 tex_coords;
} Out;

void main()
{
  gl_Position = vec4(vPosition, 1);

  Out.tex_coords = vTexCoords;
}
