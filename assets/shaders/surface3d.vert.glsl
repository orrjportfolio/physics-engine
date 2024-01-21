#version 460

in layout(location=0) vec3 v_pos;
in layout(location=1) vec3 v_norm;
in layout(location=2) vec2 v_uv;

out vec3 f_pos;
out vec3 f_norm;
out vec2 f_uv;

uniform mat4
	u_modelMat,
	u_viewMat,
	u_projMat;

void main() {
	mat4 modelViewMat = u_viewMat * u_modelMat;
	mat4 normMat = transpose(inverse(modelViewMat));
	
	gl_Position = u_projMat * modelViewMat * vec4(v_pos, 1.0);
	
	f_norm = (normMat * vec4(v_norm, 1.0)).xyz;
	f_uv = v_uv;
}
