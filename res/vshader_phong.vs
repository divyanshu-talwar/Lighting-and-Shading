#version 330

layout (location = 0)in vec3 vVertex;
layout (location = 1)in vec3 vColor;
layout (location = 2)in vec3 vNormal;

uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;

out vec3 position_eye, normal_eye;
void main(void) {
	position_eye = vec3 (vView * vModel * vec4 (vVertex, 1.0));
	// normal_eye = vec3 (vView * vModel * vec4 (vNormal, 0.0));
	normal_eye = vec3 (vView * (transpose(inverse(vModel))) * vec4 (vNormal, 0.0));
	gl_Position = vProjection * vec4 (position_eye, 1.0);
}
