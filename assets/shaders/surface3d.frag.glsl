#version 460

in vec3 f_norm;
in vec2 f_uv;

out vec3 o_colour;

uniform sampler2D u_tex;
uniform vec3 u_colour;

void main() {
	vec3 norm = normalize(f_norm);
	
	const vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
	float light = (max(0, dot(norm, lightDir)) * 0.6) + 0.4;
	
	vec3 albedo = u_colour * texture2D(u_tex, f_uv).rgb;
	
	o_colour = albedo * light;
}
