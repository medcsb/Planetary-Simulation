#version 330 core

uniform vec3 camPos;

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoord;
flat in int fIsSun;

out vec4 color;

struct Material {
    sampler2D albedoTex;
};

uniform Material material;
uniform bool isSun;

void main() {
    if (isSun) {
        // Sun's own rendering - make it emit light
        vec3 texColor = texture(material.albedoTex, fTexCoord).rgb;
        // Add emission to make the sun glow
        vec3 emission = vec3(1.0, 0.9, 0.7) * 1.5; // Warm yellow-white glow
        color = vec4(texColor * emission, 1.0);
        return;
    }
    
    // For other objects (Earth and Moon)
    vec3 n = normalize(fNormal);
    // Light direction is just the normalized negative of the current position
    // since we know the sun is at (0,0,0)

    vec3 l = normalize(fPosition);
    vec3 v = normalize(camPos - fPosition);
    vec3 r = reflect(-l, n);

    // Calculate distance to sun (simply the length of position vector since sun is at origin)
    float distance = length(fPosition);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);

    vec3 ambient = vec3(0.1, 0.1, 0.1);
    
    float diff = max(dot(n, l), 0.0);
    vec3 sunColor = vec3(1.0, 0.95, 0.8); // Warm sunlight color
    vec3 diffuse = diff * sunColor;

    float spec = pow(max(dot(v, r), 0.0), 32.0);
    vec3 specular = spec * sunColor;

    vec3 texColor = texture(material.albedoTex, fTexCoord).rgb;
    
    // Apply attenuation to diffuse and specular components
    color = vec4((ambient + (diffuse + specular)) * texColor, 1.0);
}
