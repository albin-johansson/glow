#version 450 core

precision highp float;

layout (location = 0) in VsOutput {
  vec3 ws_position;
  vec3 vs_position;
  vec3 vs_normal;
  vec2 tex_coords;
} In;

layout (location = 0) out vec4 frag_color;

layout (binding = 5) uniform sampler2D material_diffuse_tex;

layout (std140, binding = 1) uniform MaterialBuffer {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  vec4 emission;
  bool has_ambient_tex;
  bool has_diffuse_tex;
  bool has_specular_tex;
  bool has_emission_tex;
} Material;

void main()
{
  if (Material.has_diffuse_tex) {
    frag_color = texture(material_diffuse_tex, In.tex_coords);
  }
  else {
    frag_color = vec4(Material.diffuse.rgb, 1.0);
  }
}
