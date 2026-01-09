#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec3 aInstanceTranslation;

uniform mat4 uProjectionView;

out vec3 vNormal;
out vec2 vUv;

void main() {
    vNormal = aNormal;
    vUv = aUV;
    mat4 model = mat4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, aInstanceTranslation.x, aInstanceTranslation.y, aInstanceTranslation.z, 1.0);
    gl_Position = uProjectionView * model * vec4(aPos, 1.0);
}
