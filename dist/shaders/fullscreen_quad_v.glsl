#version 330
// draws a quad fullscreen.
// Namely, draws the points (-1, -1), (1, -1), (-1, 1), (1, 1).
// To properly use this shader, draw 4 verts with mode GL_TRIANGLE_STRIP.
void main() {
	gl_Position = vec4(2 * (gl_VertexID & 1) - 1, (gl_VertexID & 2) - 1, 0.0, 1.0);
}
