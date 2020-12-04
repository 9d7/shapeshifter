#version 330

uniform uvec2 ViewportSize;
uniform uvec2 TextureSize;
uniform vec2 Camera;
uniform float Parallax;

uniform sampler2D TEX;
out vec4 fragColor;

void main() {

	// The scaling algorithm works like this:
	// First, nearest neighbor scale TEX to an integer multiple size
	// that is larger than ViewportSize.
	// Then, bilinearly sample that larger texture.
	float scale = ceil(float(ViewportSize.x) / float(TextureSize.x));
	vec2 scaled_texture_size = TextureSize * scale;

	vec2 loc = gl_FragCoord.xy;
	loc *= scaled_texture_size / ViewportSize;

	loc += scale * fract(Camera * Parallax);
	vec2 frac = fract(loc);

	uvec2 loc0 = uvec2(floor(floor(loc) / scale));
	uvec2 loc1 = uvec2(floor(ceil(loc)  / scale));

	vec3 tex00 = texelFetch(TEX, ivec2(loc0.x, loc0.y), 0).rgb;
	vec3 tex01 = texelFetch(TEX, ivec2(loc0.x, loc1.y), 0).rgb;
	vec3 tex10 = texelFetch(TEX, ivec2(loc1.x, loc0.y), 0).rgb;
	vec3 tex11 = texelFetch(TEX, ivec2(loc1.x, loc1.y), 0).rgb;

	vec3 tex0 = mix(tex00, tex10, frac.x);
	vec3 tex1 = mix(tex01, tex11, frac.x);

	vec3 color = mix(tex0, tex1, frac.y);

	fragColor = vec4(color, 1.0);

}
