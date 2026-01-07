#version 460 core

in vec3 vNormal;
in vec2 vUv;

out vec4 FragColor;

void main() {
    if(vUv.x >= 0.05 && vUv.x <= 0.95 && vUv.y >= 0.05 && vUv.y <= 0.95) {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    } else {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
