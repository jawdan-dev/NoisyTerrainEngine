#pragma vertex

in uint v_packedVertex;

in vec3 i_position;

uniform mat4 u_viewProjection;

out vec4 vertexColor;

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
}

#pragma fragment

out vec4 f_color;

in vec4 vertexColor;

void main() {
	// Set color.
	f_color = vertexColor;
}