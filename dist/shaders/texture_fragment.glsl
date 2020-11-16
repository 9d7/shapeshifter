#version 330

uniform sampler2D TEX;
in vec2 texCoord;
out vec4 fragColor;

void main() {
	// I don't know why this needs to be scaled, but it does?
	fragColor = texture(TEX, (texCoord + vec2(1.0, 1.0)) / 2.0);
}
