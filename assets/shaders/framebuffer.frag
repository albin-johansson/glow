#version 410 core

precision highp float;

out vec4 frag_color;

/* 0 */ uniform sampler2D framebuffer;

uniform float gamma = 2.2;
uniform bool use_gamma_correction = false;

void main()
{
  vec2 framebuffer_size = textureSize(framebuffer, 0);
  frag_color = texture(framebuffer, gl_FragCoord.xy / framebuffer_size);

  if (use_gamma_correction) {
    frag_color.xyz = pow(frag_color.xyz, vec3(1.0 / gamma));
  }
}
