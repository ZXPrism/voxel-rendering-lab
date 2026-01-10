#version 460 core
out vec4 FragColor;

in vec3 vUVW;

uniform samplerCube uSkyboxTexture;

void main() {
    FragColor = texture(uSkyboxTexture, vUVW);
}
