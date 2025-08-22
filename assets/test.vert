#version 460 core

layout(location = 0) in vec3 aPos;

uniform vec3 color;

void main() {
	gl_Position = vec4(aPos, 1.0);
}
