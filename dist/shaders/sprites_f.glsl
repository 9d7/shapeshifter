#version 330

uniform sampler2D TEX;

flat in vec2 size;
flat in vec2 texCoords;
in vec2  uv;

out vec4 fragColor;

void main() {
	fragColor = texelFetch(TEX, ivec2(texCoords) + ivec2(size * uv), 0).rgba;
}
