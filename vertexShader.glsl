#version 330 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoord;

uniform mat4 viewMat, projMat, modelMat;

out vec3 fNormal;
out vec2 fTexCoord;
out vec3 fPosition;

void main() 
{
    vec4 worldPos = modelMat * vec4(vPosition, 1.0);
    gl_Position = projMat * viewMat * worldPos;
    
    fNormal = mat3(modelMat) * vNormal;
    fPosition = worldPos.xyz;
    fTexCoord = vTexCoord;
}