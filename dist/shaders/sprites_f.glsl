#version 330

uniform sampler2D TEX;

in uvec2 size;
in uvec2 texCoords;
in vec2  uv;

out vec4 fragColor;

void main() {
	fragColor = texture(TEX, vec2(texCoords) + vec2(size) * uv, 0).rgba;
}
