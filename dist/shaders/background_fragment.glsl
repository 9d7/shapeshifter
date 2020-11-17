#version 330

in vec2 texCoord;
out vec4 fragColor;
uniform float Time;
uniform ivec2 Camera;

#define RESOLUTION vec2(320, 240)
#define MOD3 vec3(0.1031, .11369, .13787)
vec3 hash33(vec3 p3) {
	p3	= fract(p3 * MOD3);
	p3 += dot(p3, p3.yzx + 19.19);
	return -1.0 + 2.0 * fract(vec3((p3.x + p3.y)*p3.z, (p3.x+p3.z)*p3.y, (p3.y+p3.z)*p3.x));
}

float simplex_noise(vec3 p) {
	const float K1 = 0.333333333;
	const float K2 = 0.166666667;

	vec3 i = floor(p + (p.x + p.y + p.z) * K1);
	vec3 d0 = p - (i - (i.x + i.y + i.z) * K2);

	// thx nikita: https://www.shadertoy.com/view/XsX3zB
	vec3 e = step(vec3(0.0), d0 - d0.yzx);
	vec3 i1 = e * (1.0 - e.zxy);
	vec3 i2 = 1.0 - e.zxy * (1.0 - e);

	vec3 d1 = d0 - (i1 - 1.0 * K2);
	vec3 d2 = d0 - (i2 - 2.0 * K2);
	vec3 d3 = d0 - (1.0 - 3.0 * K2);

	vec4 h = max(0.6 - vec4(dot(d0, d0), dot(d1, d1), dot(d2, d2), dot(d3, d3)), 0.0);
	vec4 n = h * h * h * h * vec4(dot(d0, hash33(i)), dot(d1, hash33(i + i1)), dot(d2, hash33(i + i2)), dot(d3, hash33(i + 1.0)));

	return dot(vec4(31.316), n);
}

float noise_scaled_sum(vec3 p) {
	float ret = 0;
	ret += simplex_noise(1.0 * p + vec3(0, 0, 3.1592)) * 0.50;
	ret += simplex_noise(2.0 * p + vec3(0, 0, 6.1059)) * 0.25;
	ret += simplex_noise(4.0 * p + vec3(0, 0, 10.259)) * 0.125;
	return ret;
}

float hatch_interpolate(float amt, ivec2 coord) {
	uint x = uint(mod(coord.x, 2));
	uint y = uint(mod(coord.y, 2));

	mat2 thresholds;
	thresholds[0] = vec2(1.0, 4.0);
	thresholds[1] = vec2(3.0, 2.0);

	return step(thresholds[x][y] / 5.0, amt);

};

void main() {

	const float PARALLAX_SPEED = 0.75f;
	ivec2 pixel_location = ivec2(gl_FragCoord.xy) + ivec2(Camera * PARALLAX_SPEED);

	float n = (noise_scaled_sum(1.0f * vec3(pixel_location / RESOLUTION, Time / 64.0f))) * 1.5;

	const vec3 color1 = vec3(7.0, 4.0, 20.0) / 255.0;
	const vec3 color2 = vec3(40.0, 9.0, 80.0) / 255.0;

	float NUM_STEPS = 3.0;
	float n_whole = floor(n * NUM_STEPS);
	float n_frac = fract(n * NUM_STEPS);

	float amt = (n_whole + hatch_interpolate(n_frac, ivec2(gl_FragCoord.xy))) / NUM_STEPS;
	vec3 color = (1.0 - amt) * color1 + amt * color2;

	fragColor = vec4(color, 1.0f);
}
