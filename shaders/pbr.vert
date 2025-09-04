#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 tangent;   // optional if you want proper normal mapping
layout(location = 4) in vec3 bitangent; // optional if you want proper normal mapping

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = model * vec4(pos, 1.0);
    FragPos = worldPos.xyz;

    // Normal matrix
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalize(normalMatrix * normal);

    // For normal mapping
    vec3 T = normalize(normalMatrix * tangent);
    vec3 B = normalize(normalMatrix * bitangent);
    vec3 N = normalize(Normal);
    TBN = mat3(T, B, N);

    TexCoords = texCoords;
    gl_Position = projection * view * worldPos;
}