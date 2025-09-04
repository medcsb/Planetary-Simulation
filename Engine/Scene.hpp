#pragma once

#include "Photon/Renderer.h"
#include "Shader.hpp"
#include "Model.hpp"

struct DummyVert {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

struct Obj {
    RenderType type;
    size_t idx;
    size_t simpleIdx = UINT32_MAX;
    size_t pbrIdx = UINT32_MAX;
};

class Scene {
private:

    size_t m_objCount = 0;
    size_t m_pbrCount = 0;
    size_t m_simpleCount = 0;

    std::vector<Shader> m_shaderPrograms;

    std::vector<PBR_Renderable> m_pbrRenderables;
    std::vector<Obj> m_objects;
    std::vector<Model> m_models;
    std::vector<std::string> m_objNames;
    SkyBox m_skyBox;
    RenderInfo m_renderInfo;
    
public:
    Scene();
    ~Scene();

    void cleanup();

    void setViewMatrix(const glm::mat4& view) { m_renderInfo.viewMatrix = view; }
    void setProjectionMatrix(const glm::mat4& projection) { m_renderInfo.projectionMatrix = projection; }

    RenderInfo& getRenderInfo() { return m_renderInfo; }
    std::vector<std::string>* getObjNames() { return &m_objNames; }
    std::vector<PBR_Renderable>* getPBRRenderables() { return &m_pbrRenderables; }
    SkyBox* getSkyBox() { return &m_skyBox; }
    std::vector<Obj>* getObjects() { return &m_objects; }

    void addLight();
    
    void AddBarelObj();
    void AddPBRCubeObj();
    void AddSphereObj();

    void AddSkyBox();

    void deleteObj(size_t idx);
    
    void initExample();
    
    private:
    VAOConfig createConfig(size_t idx);
    VAOConfig createPBRConfig(size_t idx);
    
    std::vector<DummyVert> getDummyVerts(std::vector<Vertex>& vertices);
};

