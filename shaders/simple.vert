#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 worldPos = model * vec4(pos, 1.0);
    fragPos = vec3(worldPos);
    fragNormal = mat3(transpose(inverse(model))) * normal; 
    fragTexCoords = texCoords;

    gl_Position = projection * view * model * vec4(pos, 1.0);
}