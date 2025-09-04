#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "Texture.hpp"
#include "Buffer.hpp"

#include "RenderStructs.hpp"

struct PBR_UBO {
    glm::vec3 color{1.0f, 1.0f, 1.0f};
    float attenuationFactor;

    float ambientIntensity;
    float gamma;
    float roughness;
    float metallic;

    float ao;
    int flags;
    float pad[2];
};

struct PBR_Material {
    PBR_UBO ubo;
    Texture albedoTexture;
    Texture normalTexture;
    Texture roughnessTexture;
    Texture metallicTexture;
    Texture aoTexture;
};

struct PBR_Renderable {
    MeshBuffer meshBuffer;
    PBR_Material material;
    Transform transform;
};

class PBR_RS {
private:
    GLuint m_shaderProgram;
    GLuint m_materialUBO;
    std::vector<PBR_Renderable>* m_renderableQueue;
public:
    PBR_RS();
    ~PBR_RS();

    void cleanup();

    void init(GLuint shaderProgram);

    void setRenderables(std::vector<PBR_Renderable>* renderables) { m_renderableQueue = renderables; }

    void render(RenderInfo& renderInfo);
};