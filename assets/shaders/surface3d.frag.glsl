#version 460

in vec3 f_norm;
in vec2 f_uv;

out vec3 o_colour;

uniform sampler2D u_tex;
uniform vec3 u_colour;

void main() {
	const vec3 lightColour = vec3(1.0, 1.0, 1.0);
	vec3 lightDir = normalize(vec3(0.5, 1.0, 0.75));
	
	vec3 norm = normalize(f_norm);
	
	vec3 ambient = vec3(0.2);
	float diffuse = max(dot(lightDir, norm), 0.0);
	vec3 light = mix(ambient, lightColour, diffuse);
	
	vec3 albedo = u_colour * texture2D(u_tex, f_uv).rgb;
	
	o_colour = albedo * light;
}
