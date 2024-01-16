#version 460

in layout(location=0) vec3 v_pos;
in layout(location=1) vec3 v_norm;
in layout(location=2) vec3 v_colour;
in layout(location=3) vec2 v_uv;

out vec3 f_pos;
out vec3 f_norm;
out vec3 f_colour;
out vec2 f_uv;

uniform mat4 u_mat;

void main() {
	gl_Position = u_mat * vec4(v_pos, 1.0);
	
	f_pos = v_pos;
	f_norm = v_norm;
	f_colour = v_colour;
	f_uv = v_uv;
}
