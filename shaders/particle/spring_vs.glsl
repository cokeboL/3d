#version 330

in vec3 pos;
in float intensity;
in int alive;

out float intensity_out;
out int alive_out;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main(void) {
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(pos, 1.0);
	intensity_out = intensity;
	alive_out = alive;
}