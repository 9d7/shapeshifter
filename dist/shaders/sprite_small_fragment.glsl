#version 330

uniform sampler2D TEX;
in vec2 texCoord;
in vec4 color;
out vec4 fragColor;

void main() {
	fragColor = texture(TEX, texCoord) * color;
}
