#version 330

layout (location = 0) in vec2 Position;

uniform ivec2 Camera;
uniform mat4 ScreenToClip;
uniform uvec2 AtlasSize;

#define NUM_INSTANCES 100

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

uniform ivec2 TexCoords[NUM_INSTANCES];
uniform vec4  Colors[NUM_INSTANCES];
uniform ivec2 Positions[NUM_INSTANCES];
uniform float Parallax[NUM_INSTANCES];

out vec2 texCoord;
out vec4 color;

void main() {
	vec2 pos = Positions[gl_InstanceID] - vec2(ivec2(Parallax[gl_InstanceID] * Camera));
	pos = mod(pos, vec2(SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2));
	pos -= vec2(8.0, 8.0);
	pos += Position.xy;

	gl_Position = ScreenToClip * vec4(pos, 0.0, 1.0);
	texCoord = vec2(Position.xy + TexCoords[gl_InstanceID]) / AtlasSize;
	color = Colors[gl_InstanceID];
}
