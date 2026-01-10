#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec3 aInstanceTranslation;
layout (location = 4) in int aInstanceTexture; // 0: grass block, 1: dirt block, 2: stone block

uniform mat4 uProjectionView;

out vec3 vPos;
out vec3 vNormal;
out vec2 vUv;
flat out int fTexture;
flat out int fFaceID; // 0: top, 1: bottom, 2: side

void main() {
    vNormal = aNormal;
    vUv = aUV;

    mat4 model = mat4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, aInstanceTranslation.x, aInstanceTranslation.y, aInstanceTranslation.z, 1.0);
    gl_Position = uProjectionView * model * vec4(aPos, 1.0);
    vPos = gl_Position.xyz / gl_Position.w;

    fTexture = aInstanceTexture;

    if (gl_VertexID >= 8 && gl_VertexID < 12) {
        fFaceID = 0;
    } else if (gl_VertexID >= 12 && gl_VertexID < 16) {
        fFaceID = 1;
    } else {
        fFaceID = 2;
    }
}
