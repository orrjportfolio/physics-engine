#version 460 core

in layout(location=0) vec3 v_pos;
in layout(location=1) vec3 v_norm;
in layout(location=2) vec2 v_uv;

out vec3 f_pos;
out vec3 f_light;
out vec2 f_uv;

uniform mat4
	u_modelMat,
	u_viewMat,
	u_projMat;

void main() {
	gl_Position =
		u_projMat *
		u_viewMat *
		u_modelMat *
		vec4(v_pos, 1.0);
	
	mat3 normMat = mat3(transpose(inverse(u_modelMat)));
	vec3 norm = normalize(normMat * v_norm);
	
	vec3 lightColour = vec3(1.0, 1.0, 1.0);
	vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
	
	vec3 ambient = vec3(0.5);
	float diffuse = max(dot(norm, lightDir), 0.0);
	vec3 light = mix(ambient, lightColour, diffuse);
	
	f_pos = v_pos;
	f_light = light;
	f_uv = v_uv;
}
