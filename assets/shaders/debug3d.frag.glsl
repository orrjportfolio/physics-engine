#version 460

out vec3 o_colour;

uniform vec3 u_colour;

void main() {
	o_colour = u_colour;
}
