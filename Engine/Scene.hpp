#pragma once

#include "Photon/Renderer.h"
#include "Shader.hpp"
#include "Model.hpp"
#include "physics.hpp"

struct DummyVert {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

class Scene {
private:
    Physics& m_physics;

    bool m_paused = false;

    size_t m_pbrCount = 0;

    std::vector<Shader> m_shaderPrograms;
    std::vector<PBR_Renderable> m_pbrRenderables;
    std::vector<Model> m_models;
    std::vector<PBR_Texture> m_textures;
    std::vector<std::string> m_objNames;
    SkyBox m_skyBox;
    RenderInfo m_renderInfo;
    
public:
    static inline uint32_t EARTH_TEXTURE = 0;

    Scene(Physics& physics) : m_physics(physics) {}
    ~Scene();

    void cleanup();

    void pause() { m_paused = !m_paused; }
    bool isPaused() const { return m_paused; }

    void setViewMatrix(const glm::mat4& view) { m_renderInfo.viewMatrix = view; }
    void setProjectionMatrix(const glm::mat4& projection) { m_renderInfo.projectionMatrix = projection; }

    RenderInfo& getRenderInfo() { return m_renderInfo; }
    std::vector<std::string>* getObjNames() { return &m_objNames; }
    std::vector<PBR_Renderable>* getPBRRenderables() { return &m_pbrRenderables; }
    SkyBox* getSkyBox() { return &m_skyBox; }
    Physics* getPhysics() { return &m_physics; }
    size_t getObjCount() const { return m_pbrCount; }

    void initExample();
    void AddLight();
    void AddSphereObj();
    void AddPlanetObj();
    void AddSkyBox();
    void deleteObj(size_t idx);
    void clear();
    void update(float dt);
    
    private:
    VAOConfig createConfig(size_t idx);
    VAOConfig createPBRConfig(size_t idx);

    void loadTextures();
    
    std::vector<DummyVert> getDummyVerts(std::vector<Vertex>& vertices);
};

