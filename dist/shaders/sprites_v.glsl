#version 330

uniform vec2 Camera = vec2(0.0, 0.0);
uniform vec2 ScreenSize;

layout(location = 0) in vec2      Position;
layout(location = 1) in float     Rotation;
layout(location = 2) in vec2      Size;
layout(location = 3) in vec2      TexCoords;

flat out vec2    size;
flat out vec2    texCoords;
out vec2     uv;

void main() {

	int VertexID = gl_VertexID % 6;
	int vert = clamp(int(VertexID) - 1, 0, 3);

	uv = vec2(vert & 1, (vert & 2) / 2);

	mat2 rot = mat2(
		cos(Rotation), -sin(Rotation),
		sin(Rotation),  cos(Rotation)
	);

	gl_Position = vec4((Size * 2 * (rot * (uv - vec2(0.5, 0.5))) + Position) / ScreenSize, 0.0, 1.0);

	size = Size;
	texCoords = TexCoords;

}
