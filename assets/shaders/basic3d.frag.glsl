#version 460

in vec3 f_pos;
in vec3 f_norm;
in vec3 f_colour;
in vec2 f_uv;

out vec3 o_colour;

void main() {
	o_colour = f_colour;
}
