#pragma vertex

out vec4 vertexColor;
out float depth;

in uint v_packedVertex;
in vec3 i_position;

uniform mat4 u_viewProjection;
uniform vec3 u_playerPosition;

void main() {
	// Get vertex information.
	vec3 position = vec3(
		float((v_packedVertex >> 0) & 0x1f),
		float((v_packedVertex >> 5) & 0x1ff),
		float((v_packedVertex >> 14) & 0x1f)
	);
	vec3 color = vec3(
		float((v_packedVertex >> 19) & 0xf) / float(0xf),
		float((v_packedVertex >> 23) & 0x1f) / float(0x1f),
		float((v_packedVertex >> 28) & 0xf) / float(0xf)
	);

	// Send to next stage.
	gl_Position = u_viewProjection * vec4(
		position.xyz + i_position.xyz,
		1.0f
	);
	vertexColor = vec4(color.rgb, 1.0f);

	// Calculate depth.
	//vec3 diff = u_playerPosition - (position + i_position);
	//depth = dot(diff, diff);
	depth = distance(u_playerPosition, position + i_position);
}

#pragma fragment

out vec4 f_color;

in vec4 vertexColor;
in float depth;

uniform float u_renderDistance;

float getFogFactor(float d) {
    const float fogMax = u_renderDistance * u_renderDistance;
    const float fogMin = 50.0f * 50.0f;
    return clamp(1 - (fogMax - d) / (fogMax - fogMin), 0.0f, 1.0f);
}

void main() {
	const float fogAmount = getFogFactor(depth);

	// Set color.
	const vec4 fogColor = vec4(0.45f, 0.6f, 1.0f, 1.0f);
	f_color = mix(vertexColor, fogColor, fogAmount);
}