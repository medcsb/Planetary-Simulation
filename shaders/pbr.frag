#version 450 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

uniform vec3 viewPos;

// Lights (simplified: single point light for now)
uniform vec3 lightPos;
uniform vec3 lightColor;

// Material UBO
layout(std140, binding = 1) uniform Material {
    float pad;
    float attenuationFactor;
    float ambientIntensity;
    float gamma;

    float metallic;
    float roughness;
    float ao;
    int flags; 
};

#define HAS_ALBEDO_TEX     0x01 // bit 1
#define HAS_NORMAL_TEX     0x02 // bit 2
#define HAS_METALLIC_TEX   0x04 // bit 3
#define HAS_ROUGHNESS_TEX  0x08 // bit 4
#define HAS_AO_TEX         0x10 // bit 5

// Textures
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

// ----------------------------------------------------------------------------
// Helpers
vec3 getNormalFromMap() {
    if ((flags & HAS_NORMAL_TEX) == 0) {
        return normalize(Normal);
    }
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;
    return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / (3.14159265 * denom * denom);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness + 1.0;
    float k = (r*r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------

void main() {
    // Fetch maps
    vec3 albedo = vec3(1.0);
    if ((flags & HAS_ALBEDO_TEX) != 0) {
        albedo = pow(texture(albedoMap, TexCoords).rgb, vec3(gamma)); // sRGB → linear
    } else {
        albedo = pow(vec3(pad), vec3(gamma));
    }

    vec3 N = getNormalFromMap();
    float metallicVal = 0.0;
    if ((flags & HAS_METALLIC_TEX) != 0) {
        metallicVal = texture(metallicMap, TexCoords).r;
    } else {
        metallicVal = metallic;
    }

    float rough = 0.0;
    if ((flags & HAS_ROUGHNESS_TEX) != 0) {
        rough = texture(roughnessMap, TexCoords).r;
    } else {
        rough = roughness;
    }

    float aoVal = 1.0;
    if ((flags & HAS_AO_TEX) != 0) {
        aoVal = texture(aoMap, TexCoords).r;
    } else {
        aoVal = ao;
    }
    

    // Lighting vectors
    vec3 V = normalize(viewPos - FragPos);
    vec3 L = normalize(lightPos - FragPos);
    vec3 H = normalize(V + L);

    // Light attenuation (point light)
    float dist = length(lightPos - FragPos);
    float attenuation = attenuationFactor / (dist * dist);
    vec3 radiance = lightColor * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, rough);
    float G   = GeometrySmith(N, V, L, rough);

    vec3 F0 = mix(vec3(0.04), albedo, metallicVal);
    vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N,V), 0.0) * max(dot(N,L), 0.0) + 0.001;
    vec3 specular     = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallicVal;

    float NdotL = max(dot(N, L), 0.0);
    vec3 Lo = (kD * albedo / 3.14159265 + specular) * radiance * NdotL;

    // Ambient (IBL would be better, but keep AO simple here)
    vec3 ambient = ambientIntensity * albedo * aoVal;

    vec3 color = ambient + Lo;

    // gamma correction
    color = pow(color, vec3(1.0/gamma));
    FragColor = vec4(color, 1.0);
}
