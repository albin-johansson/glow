#version 410 core

precision highp float;

in VsOut {
  vec2 tex_coords;
} In;

out vec4 frag_color;

uniform sampler2D uEnvTexture;

layout (std140) uniform EnvironmentBuffer {
  mat4 uInverseProjViewMatrix;
  vec4 uCameraPos;
  float uBrightness;
  float uGamma;
  bool uGammaCorrectionEnabled;
};

#define PI 3.14159265359

void main()
{
  // Calculate the world-space position of this fragment on the near plane
  vec4 ws_frag_pos = uInverseProjViewMatrix * vec4(In.tex_coords * 2.0 - 1.0, 1.0, 1.0);
  ws_frag_pos *= (1.0 / ws_frag_pos.w);

  // Calculate the world-space direction from the camera to that position
  vec3 ws_dir_to_frag = normalize(ws_frag_pos.xyz - uCameraPos.xyz);

  // Calculate the spherical coordinates of the direction
  float theta = acos(max(-1.0, min(1.0, ws_dir_to_frag.y)));
  float phi = atan(ws_dir_to_frag.z, ws_dir_to_frag.x);
  if (phi < 0) {
    phi += 2.0 * PI;
  }

  // Use these to lookup the color in the environment map
  vec2 texel = vec2(phi / (2.0 * PI), 1 - theta / PI);
  frag_color = uBrightness * texture(uEnvTexture, texel);

  if (uGammaCorrectionEnabled) {
    frag_color.xyz = pow(frag_color.xyz, vec3(1.0 / uGamma));
  }
}
