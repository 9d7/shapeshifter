#version 330

uniform sampler2D TEX;

uniform ivec2 ScreenSize;
uniform ivec2 ViewportSize;

out vec4 fragColor;

flat in vec2 texCoords;
in vec2 uv;
in vec4 color;

void main() {

	float scale = ceil(float(ViewportSize.x) / float(ScreenSize.x));
	vec2 scaled_texture_size = scale * vec2(8.0, 8.0);
	vec2 loc = uv * scaled_texture_size; // 0 -> scale * size

	vec2 frac = fract(loc);

	ivec2 loc0 = ivec2(floor(floor(loc) / scale));
	ivec2 loc1 = ivec2(floor(ceil(loc)  / scale));

	ivec2 tile0 = loc0 + ivec2(texCoords);
	ivec2 tile1 = loc1 + ivec2(texCoords);

	vec4 tex00 = texelFetch(TEX, ivec2(tile0.x, tile0.y), 0).rgba;
	vec4 tex01 = texelFetch(TEX, ivec2(tile0.x, tile1.y), 0).rgba;
	vec4 tex10 = texelFetch(TEX, ivec2(tile1.x, tile0.y), 0).rgba;
	vec4 tex11 = texelFetch(TEX, ivec2(tile1.x, tile1.y), 0).rgba;


	if (loc0.x < 0.0 || loc0.x >= 8.0) {
		tex00 = vec4(0.0, 0.0, 0.0, 0.0);
		tex01 = vec4(0.0, 0.0, 0.0, 0.0);
	}
	if (loc0.y < 0.0 || loc0.x >= 8.0) {
		tex00 = vec4(0.0, 0.0, 0.0, 0.0);
		tex10 = vec4(0.0, 0.0, 0.0, 0.0);
	}
	if (loc1.x < 0.0 || loc1.x >= 8.0) {
		tex10 = vec4(0.0, 0.0, 0.0, 0.0);
		tex11 = vec4(0.0, 0.0, 0.0, 0.0);
	}
	if (loc1.y < 0.0 || loc1.y >= 8.0) {
		tex01 = vec4(0.0, 0.0, 0.0, 0.0);
		tex11 = vec4(0.0, 0.0, 0.0, 0.0);
	}


	// https://en.wikipedia.org/wiki/Alpha_compositing#Straight_versus_premultiplied
	tex00.rgb *= tex00.a;
	tex01.rgb *= tex01.a;
	tex10.rgb *= tex10.a;
	tex11.rgb *= tex11.a;

	vec4 tex0 = mix(tex00, tex10, frac.x);
	vec4 tex1 = mix(tex01, tex11, frac.x);

	vec4 out_color = mix(tex0, tex1, frac.y);
	if (out_color.a != 0.0) {
		out_color.rgb /= out_color.a;
	}

	fragColor = out_color.rgba * color;
}
