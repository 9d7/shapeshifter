#version 330

#define NUM_STARS 100U

uniform vec2 Camera;
uniform ivec2 ScreenSize;

uniform uint NumStars = NUM_STARS;
uniform vec2 TexCoords[NUM_STARS];

flat out vec2 texCoords;
out vec2 uv;
out vec4 color;

// https://www.shadertoy.com/view/ltB3zD
const float PHI = 1.61803398874989484820459; // Golden Ratio

float gold_noise(in float x, in float seed)
{
	return fract(tan(distance(x*PHI, x)*seed)*x);
}

void main() {

	uv = vec2(gl_VertexID & 1, (gl_VertexID & 2) / 2);

	vec2 pos = vec2(gold_noise(gl_InstanceID + 1.0, 1.0), gold_noise(gl_InstanceID + 1.0, 2.0));
	pos *= ScreenSize * 2;

	float parallax = gold_noise(gl_InstanceID + 1.0, 3.0) * 0.70 + 0.25;
	pos -= Camera * parallax;

	pos = mod(pos, ScreenSize * 2);
	pos -= ScreenSize;
	pos -= vec2(8.0, 8.0);

	gl_Position = vec4((8.0 * (uv - vec2(0.5, 0.5)) + pos) / (ScreenSize / 2), 0.0, 1.0);

	texCoords = TexCoords[gl_InstanceID];

	color = vec4(
		1.0f,
		gold_noise(gl_InstanceID + 1.0, 4.0) * 0.25 + 0.75,
		gold_noise(gl_InstanceID + 1.0, 5.0) * 0.5,
		parallax - 0.15
	);

}
