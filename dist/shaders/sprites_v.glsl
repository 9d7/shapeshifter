#version 330

in vec2  Position;
in float Rotation;
in uvec2 Size;
in uvec2 Texture;
in uint  VertexID;

out uvec2 size;
out uvec2 texCoords;
out vec2  uv;

void main() {

	uint vert = (VertexID == 5) ? 3 : (VertexID == 0) ? 0 : VertexID - 1;
	uv = vec2(vert & 1, (vert & 2) / 2);

	mat2 rot = mat2(
		cos(Rotation), -sin(Rotation),
		sin(Rotation),  cos(Rotation)
	);

	gl_Position = vec2(Size) * (rot * (uv - vec2(0.5, 0.5))) + Position;
	size = Size;
	texCoords = Texture;

}
