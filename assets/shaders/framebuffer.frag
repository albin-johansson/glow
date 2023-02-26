#version 410 core

precision highp float;

out vec4 frag_color;

/* 0 */ uniform sampler2D framebuffer;

void main()
{
  vec2 framebuffer_size = textureSize(framebuffer, 0);
  frag_color = texture(framebuffer, gl_FragCoord.xy / framebuffer_size);
}
