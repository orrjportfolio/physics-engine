#version 460 core

in layout(location=0) vec3 v_pos;
in layout(location=1) vec3 v_colour;

out vec3 f_colour;

uniform mat4
	u_viewMat,
	u_projMat;

void main() {
	gl_Position =
		u_projMat *
		u_viewMat *
		vec4(v_pos, 1.0);
	
	f_colour = v_colour;
}
