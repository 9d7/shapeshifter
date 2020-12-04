#version 330

uniform uvec2 FieldSize;
uniform uvec2 ScreenSize;


void main() {
	vec2 position = vec2((gl_VertexID & 1), (gl_VertexID & 2) / 2.0);

	if (gl_InstanceID == 0) {
		position.y *= float(ScreenSize.y - FieldSize.y) / float(ScreenSize.y);
		position.y += float(FieldSize.y) / float(ScreenSize.y);
	} else {
		position.x *= float(ScreenSize.x - FieldSize.x) / float(ScreenSize.x);
		position.x += float(FieldSize.x) / float(ScreenSize.x);
	}

	position *= 2.0;
	position -= vec2(1.0, 1.0);

	gl_Position = vec4(position, 0.0, 1.0);
}
