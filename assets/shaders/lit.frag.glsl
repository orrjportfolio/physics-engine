#version 460 core

in vec3 f_pos;
in vec3 f_light;
in vec2 f_uv;

out vec3 o_colour;

void main() {
	vec3 albedo = vec3(1.0, 1.0/3.0, 2.0/3.0);
	
	o_colour = f_light * albedo;
}
