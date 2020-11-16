#version 330

in vec2 texCoord;
out vec4 fragColor;
uniform float Time;


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

float hatch_interpolate(float amt, uvec2 coord) {
	uint x = uint(mod(coord.x, 4));
	uint y = uint(mod(coord.y, 4));

	mat4 thresholds;
	thresholds[0] = vec4( 1.0, 13.0,  4.0, 16.0);
	thresholds[1] = vec4( 9.0,  5.0, 12.0,  8.0);
	thresholds[2] = vec4( 3.0, 15.0,  2.0, 14.0);
	thresholds[3] = vec4(11.0,  7.0, 10.0,  6.0);

	return step(thresholds[x][y] / 17.0, amt);

};

void main() {
	float n = (noise_scaled_sum(1.0f * vec3(texCoord.xy, Time / 128.0f))) * 2.0;

	const vec3 color1 = vec3(4.0, 8.0, 45.0) / 255.0;
	const vec3 color2 = vec3(36.0, 22.0, 73.0) / 255.0;
	const vec3 color3 = vec3(76.0, 40.0, 119.0) / 255.0;

	float f;
	vec3 color;
	if (n < 0.5) {
		f = hatch_interpolate(n * 2.0, uvec2(gl_FragCoord.xy));
		color = f > 0.0 ? color2 : color1;
	} else {
		n -= 0.5;
		f = hatch_interpolate(n * 2.0, uvec2(gl_FragCoord.xy));
		color = f > 0.0 ? color3 : color2;
	}
	fragColor = vec4(color, 1.0f);
}
