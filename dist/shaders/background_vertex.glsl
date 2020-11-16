#version 330

layout (location = 0) in vec2 Position;
layout (location = 1) in vec2 TexCoord;
out vec2 texCoord;

void main() {
	gl_Position = vec4(Position.xy, 0.0, 1.0);
	texCoord = TexCoord;
}
