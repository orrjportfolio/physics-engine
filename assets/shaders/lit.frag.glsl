#version 460 core

in vec3 f_pos;
in vec3 f_light;
in vec2 f_uv;

out vec3 o_colour;

uniform sampler2D u_tex;
uniform vec3 u_colour;

void main() {
	vec3 albedo = texture2D(u_tex, f_uv).rgb;
	
	o_colour = f_light * albedo * u_colour;
}
