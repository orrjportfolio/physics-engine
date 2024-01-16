#version 460

in vec3 f_pos;
in vec3 f_norm;
in vec3 f_colour;
in vec2 f_uv;

out vec3 o_colour;

void main() {
	vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
	float light = (max(0, dot(f_norm, lightDir)) * 0.6) + 0.4;
	
	o_colour = f_colour * light;
}
