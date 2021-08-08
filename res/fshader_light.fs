#version 330

in vec3 fColor;
out vec4 color;
void main(void) {
	color = vec4(fColor, 1.0);
}
