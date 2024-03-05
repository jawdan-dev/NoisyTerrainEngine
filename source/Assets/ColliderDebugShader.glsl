#pragma vertex

out vec3 f_color;

in vec3 v_position;
in vec3 i_position;
in vec3 i_scale;
in vec3 i_color;

uniform mat4 u_viewProjection;

void main() {
	// Set position.
	gl_Position = u_viewProjection * vec4(
		(v_position * i_scale) + i_position,
		1.0f
	);

	// Set color.
	f_color = i_color;
}

#pragma fragment

out vec4 o_color;

in vec3 f_color;

void main() {
	o_color = vec4(f_color, 1.0f);
}