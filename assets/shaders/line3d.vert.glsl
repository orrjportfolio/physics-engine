#version 460

in vec3 v_pos;
in vec3 v_colour;

out vec3 f_pos;
out vec3 f_colour;

uniform mat4
	u_viewMat,
	u_projMat;

void main() {
	gl_Position = u_projMat * u_viewMat * vec4(v_pos, 1.0);
	
	f_pos = v_pos;
	f_colour = v_colour;
}
