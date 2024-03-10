#pragma vertex

in vec3 v_position;
in vec3 i_offset;
in float i_scale;

uniform mat4 u_viewProjection;

void main() {
	// Set position.
	gl_Position = u_viewProjection * vec4(
		(v_position * i_scale) + i_offset,
		1.0f
	);
}

#pragma fragment

out vec4 o_color;

void main() {
	o_color = vec4(0.5f, 0.8f, 0.5f, 1.0f);
}