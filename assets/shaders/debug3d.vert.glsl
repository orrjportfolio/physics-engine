#version 460

in layout(location=0) vec3 v_pos;

uniform mat4
	u_modelMat,
	u_viewMat,
	u_projMat;

void main() {
	gl_Position = u_projMat * u_viewMat * u_modelMat * vec4(v_pos, 1.0);
}
