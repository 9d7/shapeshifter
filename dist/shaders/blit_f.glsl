#version 330

uniform sampler2D TEX;
uniform uvec2 ViewportOffset;

out vec4 fragColor;

void main() {

	ivec2 loc = ivec2(gl_FragCoord.xy) - ivec2(ViewportOffset);
	fragColor = vec4(texelFetch(TEX, loc, 0).rgb, 1.0);
}
