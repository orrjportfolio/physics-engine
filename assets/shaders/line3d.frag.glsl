#version 460

in vec3 f_pos;
in vec3 f_colour;

out vec3 o_colour;

void main() {
	o_colour = f_colour;
}
