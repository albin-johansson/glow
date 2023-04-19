#version 410 core

precision highp float;

out vec4 frag_color;

uniform sampler2D uFramebuffer;

layout (std140) uniform Options {
  float uGamma;
  bool uGammaCorrectionEnabled;
};

void main()
{
  vec2 framebuffer_size = textureSize(uFramebuffer, 0);
  frag_color = texture(uFramebuffer, gl_FragCoord.xy / framebuffer_size);

  if (uGammaCorrectionEnabled) {
    frag_color.xyz = pow(frag_color.xyz, vec3(1.0 / uGamma));
  }
}
