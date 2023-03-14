#version 410 core

precision highp float;

in VsOut {
  vec2 tex_coords;
} In;

out vec4 frag_color;

/* 0 */ uniform sampler2D env_texture;

layout (std140) uniform EnvironmentBuffer {
  mat4 inverse_proj_view_matrix;
  vec4 camera_pos;
  float brightness;
  float gamma;
  bool use_gamma_correction;
};

#define PI 3.14159265359

void main()
{
  // Calculate the world-space position of this fragment on the near plane
  vec4 ws_frag_pos = inverse_proj_view_matrix * vec4(In.tex_coords * 2.0 - 1.0, 1.0, 1.0);
  ws_frag_pos *= (1.0 / ws_frag_pos.w);

  // Calculate the world-space direction from the camera to that position
  vec3 ws_dir_to_frag = normalize(ws_frag_pos.xyz - camera_pos.xyz);

  // Calculate the spherical coordinates of the direction
  float theta = acos(max(-1.0, min(1.0, ws_dir_to_frag.y)));
  float phi = atan(ws_dir_to_frag.z, ws_dir_to_frag.x);
  if (phi < 0) {
    phi += 2.0 * PI;
  }

  // Use these to lookup the color in the environment map
  vec2 texel = vec2(phi / (2.0 * PI), 1 - theta / PI);
  frag_color = brightness * texture(env_texture, texel);

  if (use_gamma_correction) {
    frag_color.xyz = pow(frag_color.xyz, vec3(1.0 / gamma));
  }
}
