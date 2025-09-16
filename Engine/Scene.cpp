#include "Scene.hpp"


//Scene::Scene() {}
Scene::~Scene() {}

void Scene::cleanup() {
    for (auto& renderable : m_pbrRenderables) {
        renderable.meshBuffer.cleanup();
    }
    m_models.clear();
    m_objNames.clear();
}

void Scene::update(float dt) {
    m_physics.update(dt);
    for (size_t i = 0; i < m_pbrCount; ++i) {
        m_pbrRenderables[i].transform.setPos(m_physics.getPlanets()->at(i).pos);
        m_pbrRenderables[i].transform.calcMatrix();
    }
}

void Scene::initExample() {
    AddSkyBox();
    AddLight();
    AddSphereObj();
    m_physics.addPlanet(m_pbrRenderables.back().transform.pos, glm::vec3(0.05f), 1.0f, 1.0f);
    AddSphereObj();
    m_pbrRenderables.back().transform.setPos(glm::vec3(-3.0f, 0.0f, 0.0f));
    m_physics.addPlanet(m_pbrRenderables.back().transform.pos, glm::vec3(-0.05f), 1.0f, 1.0f);
}

void Scene::AddPlanetObj() {
    AddSphereObj();
    m_physics.addPlanet(m_pbrRenderables.back().transform.pos, glm::vec3(0.05f), 1.0f, 1.0f);
}

void Scene::AddSphereObj() {
    if (m_pbrRenderables.size() <= m_pbrCount) {
        m_pbrRenderables.resize(m_pbrCount + 1);
    }

    m_pbrCount++;
    m_objNames.push_back("Sphere " + std::to_string(m_pbrCount));


    Model sphereModel;
    sphereModel.SphereModel();
    m_models.push_back(sphereModel);

    std::vector<DummyVert> vertData = getDummyVerts(m_models[m_pbrCount - 1].getVertices());
    VAOConfig config = createPBRConfig(m_pbrCount - 1);
    m_pbrRenderables[m_pbrCount - 1].meshBuffer = Buffer::createMeshBuffer(
        config,
        vertData.data(),
        nullptr
    );

    int flags = HAS_ALBEDO_TEX | HAS_NORMAL_TEX | HAS_METALLIC_TEX;
    m_pbrRenderables[m_pbrCount - 1].material.ubo = {
        .color = {0.2f, 0.5f, 0.8f},
        .attenuationFactor = 1.0f,
        .ambientIntensity = 0.4f,
        .gamma = 2.2f,
        .roughness = 0.5f,
        .metallic = 0.5f,
        .ao = 1.0f,
        .flags = flags,
        .pad = {0.0f, 0.0f}
    };

    m_pbrRenderables[m_pbrCount - 1].material.albedoTexture.loadTexture(std::string(TEXTURE_DIR) + "pbr_earth/albedo_map.jpg", 1);
    m_pbrRenderables[m_pbrCount - 1].material.normalTexture.loadTexture(std::string(TEXTURE_DIR) + "pbr_earth/normal_map.jpg", 2);
    m_pbrRenderables[m_pbrCount - 1].material.metallicTexture.loadTexture(std::string(TEXTURE_DIR) + "pbr_earth/metal_map.jpg", 4);

    m_pbrRenderables[m_pbrCount - 1].transform = {
        .m_matrix = glm::mat4(1.0f),
        .pos = {0.0f, 0.0f, 0.0f},
        .rot = {0.0f, 0.0f, 0.0f},
        .scale = {1.0f, 1.0f, 1.0f}
    };
    m_pbrRenderables[m_pbrCount - 1].transform.calcMatrix();
}

void Scene::AddSkyBox() {
    std::vector<float> vertData = Model::CubeVertices();
    
    m_skyBox.meshBuffer = Buffer::createMeshBuffer(
        VAOConfig{
            .attributes = {{0, 3, GL_FLOAT, false, sizeof(float) * 3, 0}},
            .size_vertex = sizeof(float) * 3,
            .num_vertices = vertData.size() / 3,
            .draw_mode = GL_TRIANGLES,
            .usage = GL_STATIC_DRAW
        },
        vertData.data()
    );
    m_skyBox.cubeMapTexture.loadCubeMap({
        std::string(TEXTURE_DIR) + "space/px.png",
        std::string(TEXTURE_DIR) + "space/nx.png",
        std::string(TEXTURE_DIR) + "space/py.png",
        std::string(TEXTURE_DIR) + "space/ny.png",
        std::string(TEXTURE_DIR) + "space/pz.png",
        std::string(TEXTURE_DIR) + "space/nz.png"
    }, 10);
}

void Scene::AddLight() {
    Light mainLight;
    
    mainLight.pos = {0.0f, 1.0f, 3.0f};
    mainLight.color = {1.0f, 1.0f, 1.0f};

    glGenVertexArrays(1, &mainLight.dummyVAO);
    m_renderInfo.lights.push_back(mainLight);

}

VAOConfig Scene::createPBRConfig(size_t idx) {
    VAOConfig config;

    config.attributes.push_back({0, 3, GL_FLOAT, false, sizeof(DummyVert), offsetof(DummyVert, pos)});
    config.attributes.push_back({1, 3, GL_FLOAT, false, sizeof(DummyVert), offsetof(DummyVert, normal)});
    config.attributes.push_back({2, 2, GL_FLOAT, false, sizeof(DummyVert), offsetof(DummyVert, texCoords)});
    config.attributes.push_back({3, 3, GL_FLOAT, false, sizeof(DummyVert), offsetof(DummyVert, tangent)});
    config.attributes.push_back({4, 3, GL_FLOAT, false, sizeof(DummyVert), offsetof(DummyVert, bitangent)}); 

    config.size_vertex = sizeof(DummyVert);
    config.num_vertices = m_models[idx].getVertices().size();
    config.index_count = UINT32_MAX;
    config.draw_mode = GL_TRIANGLES;
    config.usage = GL_DYNAMIC_DRAW;

    return config;
}


VAOConfig Scene::createConfig(size_t idx) {
    VAOConfig config;
    
    config.attributes.push_back({0, 3, GL_FLOAT, false, sizeof(Vertex), offsetof(Vertex, pos)});
    config.attributes.push_back({1, 3, GL_FLOAT, false, sizeof(Vertex), offsetof(Vertex, normal)});
    config.attributes.push_back({2, 2, GL_FLOAT, false, sizeof(Vertex), offsetof(Vertex, texCoords)});

    config.size_vertex = sizeof(Vertex);
    config.num_vertices = m_models[idx].getVertices().size();
    config.index_count = m_models[idx].getIndices().size();
    config.draw_mode = GL_TRIANGLES;
    config.usage = GL_DYNAMIC_DRAW;

    return config;
}

void Scene::deleteObj(size_t idx) {
    m_pbrRenderables[idx].meshBuffer.cleanup();
    m_pbrRenderables.erase(m_pbrRenderables.begin() + idx);
    m_pbrCount--;
    // delete the model
    m_models.erase(m_models.begin() + idx);
    // delete the object name
    m_objNames.erase(m_objNames.begin() + idx);
    // update physics planets
    m_physics.getPlanets()->erase(m_physics.getPlanets()->begin() + idx);
}

std::vector<DummyVert> Scene::getDummyVerts(std::vector<Vertex>& vertices) {
    std::vector<DummyVert> vertData;
    for (const auto& vertex : vertices) {
        DummyVert dummyVert;
        dummyVert.pos = vertex.pos;
        dummyVert.normal = vertex.normal;
        dummyVert.texCoords = vertex.texCoords;
        dummyVert.tangent = {0.0f, 0.0f, 0.0f};
        dummyVert.bitangent = {0.0f, 0.0f, 0.0f};
        vertData.push_back(dummyVert);
    }

    for (size_t i = 0; i < vertData.size(); i += 3) {
        auto& v0 = vertData[i];
        auto& v1 = vertData[i + 1];
        auto& v2 = vertData[i + 2];

        glm::vec3 edge1 = v1.pos - v0.pos;
        glm::vec3 edge2 = v2.pos - v0.pos;

        glm::vec2 deltaUV1 = v1.texCoords - v0.texCoords;
        glm::vec2 deltaUV2 = v2.texCoords - v0.texCoords;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent, bitangent;

        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

        // Accumulate for each vertex
        v0.tangent += tangent;
        v1.tangent += tangent;
        v2.tangent += tangent;

        v0.bitangent += bitangent;
        v1.bitangent += bitangent;
        v2.bitangent += bitangent;
    }

    // Normalize all tangents/bitangents
    for (auto& v : vertData) {
        v.tangent = glm::normalize(v.tangent);
        v.bitangent = glm::normalize(v.bitangent);
    }

    return vertData;
}