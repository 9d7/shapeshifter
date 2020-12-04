#version 330

uniform sampler2D TEX;
uniform vec2 ViewportSize;
uniform vec2 ScreenSize;

flat in vec2 size;
flat in vec2 texCoords;
in vec2  uv;

out vec4 fragColor;

void main() {

	float scale = ceil(ViewportSize.x / ScreenSize.x);
	vec2 scaled_texture_size = scale * size;
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


	// ensure we don't render adjacent sprites due to floating point
	// precision errors
	if (loc0.x < 0.0 || loc0.x >= size.x) {
		tex00 = vec4(0.0, 0.0, 0.0, 0.0);
		tex01 = vec4(0.0, 0.0, 0.0, 0.0);
	}
	if (loc0.y < 0.0 || loc0.y >= size.y) {
		tex00 = vec4(0.0, 0.0, 0.0, 0.0);
		tex10 = vec4(0.0, 0.0, 0.0, 0.0);
	}
	if (loc1.x < 0.0 || loc1.x >= size.x) {
		tex10 = vec4(0.0, 0.0, 0.0, 0.0);
		tex11 = vec4(0.0, 0.0, 0.0, 0.0);
	}
	if (loc1.y < 0.0 || loc1.y >= size.y) {
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

	vec4 color = mix(tex0, tex1, frac.y);
	if (color.a != 0.0) {
		color.rgb /= color.a;
	}

	fragColor = color.rgba;
}
