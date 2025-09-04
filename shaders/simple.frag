#version 450 core

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoords;

layout(location = 0) out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

layout(std140, binding = 0) uniform Material {
    vec4 baseColor;
    //
    float ambient;
    float diffuse;
    float specular;
    float specStrength;
    //
    float specPower;
    float texBlend;
    float pad0;
    float pad1;
};

uniform sampler2D tex0;

void main() {
    // Ambient
    vec3 ambientColor = ambient * lightColor;

    // Diffuse
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseColor = diffuse * diff * lightColor;

    // Specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), specPower);
    vec3 specularColor = specStrength * spec * lightColor;

    // Final color
    vec3 color = vec3(baseColor) * (1.0 - texBlend) + texture(tex0, fragTexCoords).rgb * texBlend;
    FragColor = vec4(color * (ambientColor + diffuseColor + specularColor), baseColor.w);
}
