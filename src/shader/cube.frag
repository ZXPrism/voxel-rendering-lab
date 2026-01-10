#version 460 core

in vec3 vPos;
in vec3 vNormal;
in vec2 vUv;
flat in int fTexture;
flat in int fFaceID;

out vec4 FragColor;

uniform vec3 uCameraPos;
uniform sampler2D uBlockTextureGrass;
uniform sampler2D uBlockTextureDirt;
uniform sampler2D uBlockTextureStone;

const float AMBIENT_COEFF = 0.4;
const float DIFFUSE_COEFF = 0.6;
const float SPECULAR_COEFF = 0.2;
const int SHININESS = 64;

const vec3 LIGHT_COLOR = vec3(1.0, 1.0, 1.0);
const vec3 LIGHT_POS = vec3(0.0, 20.0, 0.0);

void main() {
    vec2 uv = vUv / 2.0;
    if (fFaceID == 1) {
        uv += vec2(0.5, 0.0);
    } else if (fFaceID == 2) {
        uv += vec2(0.0, 0.5);
    }

    vec3 albedo;
    if (fTexture == 0) {
        albedo = texture(uBlockTextureGrass, uv).xyz;
    } else if (fTexture == 1) {
        albedo = texture(uBlockTextureDirt, uv).xyz;
    } else {
        albedo = texture(uBlockTextureStone, uv).xyz;
    }

    // ambient
    vec3 ambient = LIGHT_COLOR * albedo;

    // diffuse
    vec3 light = normalize(LIGHT_POS - vPos);
    vec3 diffuse = LIGHT_COLOR * albedo * max(0, dot(vNormal, light));

    // specular
    vec3 view = normalize(uCameraPos - vPos);
    vec3 half_vec = normalize(light + view);
    vec3 specular = LIGHT_COLOR * pow(max(0, dot(half_vec, vNormal)), SHININESS);

    FragColor = vec4(AMBIENT_COEFF * ambient + DIFFUSE_COEFF * diffuse + SPECULAR_COEFF * specular, 1.0);
}
