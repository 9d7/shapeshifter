#version 330

layout (location = 0) in vec2 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec4 Color;
uniform mat4 ScreenToClip;

out vec2 texCoord;
out vec4 color;

void main() {
	gl_Position = ScreenToClip * vec4(Position.xy, 0.0, 1.0);
	texCoord = TexCoord;
	color = Color;
}
