#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 uModel;
uniform mat4 uProjectionView;

out vec3 vNormal;
out vec2 vUv;

void main() {
    vNormal = aNormal;
    vUv = aUV;
    gl_Position = uProjectionView * uModel * vec4(aPos, 1.0);
}
