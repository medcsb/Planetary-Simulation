#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "Texture.hpp"
#include "Buffer.hpp"

#include "RenderStructs.hpp"

// RS stands for Render System

struct SimpleUBO {
    glm::vec4 baseColor;
    float ambient;
    float diffuse;
    float specular;
    float specStrength;
    float specPower;
    float texBlend; // blend between albedo and texture
    // padding to 64 bytes
    float pad[2];
};

struct SimpleMaterial {
    SimpleUBO ubo;
    Texture albedoTexture;
};

struct SimpleRenderable {
    MeshBuffer meshBuffer;
    SimpleMaterial material;
    Transform transform;
};

class Simple_RS {
private:
    GLuint m_shaderProgram;
    GLuint m_materialUBO;
    std::vector<SimpleRenderable>* m_renderableQueue;
public:
    Simple_RS();
    ~Simple_RS();

    void cleanup();

    void init(GLuint shaderProgram);

    void setRenderables(std::vector<SimpleRenderable>* renderables) { m_renderableQueue = renderables; }

    void render(RenderInfo& renderInfo);
};