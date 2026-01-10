#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 vUVW;

uniform mat4 uProjection;
uniform mat4 uView;

void main() {
    vUVW = aPos;
    gl_Position = uProjection * uView * vec4(aPos, 1.0);
}
