#version 330

uniform vec2 Camera;

in vec2      Position;
in float     Rotation;
in uvec2     Size;
in uvec2     Texture;
in uint      VertexID;

flat out uvec2    size;
flat out uvec2    texCoords;
out vec2     uv;

void main() {

	uint vert = VertexID - 1U;
	if (VertexID == 5U) {
		vert = 3U;
	}
	if (VertexID == 0U) {
		vert = 0U;
	}
	uv = vec2(vert & 1U, (vert & 2U) / 2U);

	mat2 rot = mat2(
		cos(Rotation), -sin(Rotation),
		sin(Rotation),  cos(Rotation)
	);

	gl_Position = vec4(vec2(Size) * (rot * (uv - vec2(0.5, 0.5))) + Position, 0.0, 1.0);
	size = Size;
	texCoords = Texture;

}
