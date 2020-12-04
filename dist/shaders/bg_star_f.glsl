#version 330

uniform sampler2D TEX;

out vec4 fragColor;

flat in vec2 texCoords;
in vec2 uv;
in vec4 color;

void main() {
	fragColor = texelFetch(TEX, ivec2(texCoords) + ivec2(8.0 * uv), 0).rgba * color;
}
