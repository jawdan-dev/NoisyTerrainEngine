#pragma vertex

out float ambience;
out float depth;
out vec3 normal;
out vec2 uv;
out vec2 uvMultiplier;

in uint v_packedVertex;
in vec3 i_position;

uniform mat4 u_viewProjection;
uniform vec3 u_playerPosition;
uniform vec2 u_voxelTextureSize;

const float ambienceFactor = 0.64f;

void main() {
	// Get vertex information.
	vec3 position = vec3(
		float((v_packedVertex >> 0) & 0x1f),
		float((v_packedVertex >> 5) & 0x1ff),
		float((v_packedVertex >> 14) & 0x1f)
	);

	// Get ambience.
	ambience = pow(ambienceFactor, float((v_packedVertex >> 19) & 0x3));

	// Get normal.
	const uint normalI = (v_packedVertex >> 21) & 0x3;
	if (normalI == 0) {
		uvMultiplier = vec2(position.z, -position.y);
		normal = vec3(1, 0, 0);
	} else if (normalI == 1) {
		normal = vec3(0, -1, 0);
		uvMultiplier = vec2(position.x, -position.z);
	} else if (normalI == 2) {
		normal = vec3(0, 0, 1);
		uvMultiplier = vec2(-position.x, -position.y);
	}
	// Check if normal on negative axis
	if (((v_packedVertex >> 23) & 0x1) == 0) {
		uvMultiplier.x = -uvMultiplier.x;
		normal = -normal;
	}

	// Get color from texture.
	const float textureIndex = float((v_packedVertex >> 24) & 0xff);
	uv = vec2(
		mod(textureIndex, u_voxelTextureSize.x),
		floor(textureIndex / u_voxelTextureSize.x)
	) / u_voxelTextureSize;

	// Set gl position.
	gl_Position = u_viewProjection * vec4(
		position.xyz + i_position.xyz,
		1.0f
	);

	// Calculate depth.
	depth = distance(u_playerPosition, position + i_position);
}

#pragma fragment

out vec4 f_color;

in float ambience;
in float depth;
in vec3 normal;
in vec2 uv;
in vec2 uvMultiplier;

uniform float u_renderDistance;
uniform sampler2D u_voxelTexture;
uniform vec2 u_voxelTextureSize;

const vec3 sunDirection = normalize(vec3(0.2f, -1.0f, 0.1f));
const vec4 shadowColor = vec4(0.0f, 0.0f, 0.4f, 1.0f);
const vec4 fogColor = vec4(0.45f, 0.6f, 1.0f, 1.0f);

float getFogFactor(float d) {
    const float fogMax = u_renderDistance;
    const float fogMin = 50.0f;
    return clamp(1 - (fogMax - d) / (fogMax - fogMin), 0.0f, 1.0f);
}

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main() {
	const float fogAmount = getFogFactor(depth);

	// Set color.
	const vec4 faceColor = texture(u_voxelTexture, uv + (fract(uvMultiplier) / u_voxelTextureSize));
	const float sunFactor = map(-dot(normal, sunDirection), -1.0f, 1.0f, 0.5, 1.0f);
	f_color = mix(mix(shadowColor, faceColor, min(sunFactor, ambience)), fogColor, fogAmount);
}