#include "Scene.hpp"


//Scene::Scene() {}
Scene::~Scene() {}

void Scene::cleanup() {
    for (auto& renderable : m_pbrRenderables) {
        renderable.meshBuffer.cleanup();
    }
    m_objects.clear();
    m_models.clear();
    m_objNames.clear();
}

void Scene::update(float dt) {
    m_physics.update(dt);
    for (auto& planet : *m_physics.getPlanets()) {
        m_pbrRenderables[planet.id].transform.setPos(planet.pos);
        m_pbrRenderables[planet.id].transform.calcMatrix();
    }
}

void Scene::initExample() {
    AddSkyBox();
    AddLight();
    AddSphereObj();
    m_physics.addPlanet(m_objects.back().pbrIdx, m_pbrRenderables.back().transform.pos, glm::vec3(0.05f), 1.0f, 1.0f);
    AddSphereObj();
    m_pbrRenderables.back().transform.setPos(glm::vec3(-3.0f, 0.0f, 0.0f));
    m_physics.addPlanet(m_objects.back().pbrIdx, m_pbrRenderables.back().transform.pos, glm::vec3(-0.05f), 1000000.5f, 1.0f);
}

void Scene::AddSphereObj() {
    if (m_pbrRenderables.size() <= m_pbrCount) {
        m_pbrRenderables.resize(m_pbrCount + 1);
    }

    Obj obj;
    obj.idx = m_objCount++;
    obj.pbrIdx = m_pbrCount++;
    m_objNames.push_back("Sphere " + std::to_string(obj.idx + 1));
    obj.type = RenderType::PBR;
    m_objects.push_back(obj);

    Model sphereModel;
    sphereModel.SphereModel();
    m_models.push_back(sphereModel);

    std::vector<DummyVert> vertData = getDummyVerts(m_models[obj.idx].getVertices());
    VAOConfig config = createPBRConfig(obj.idx);
    m_pbrRenderables[obj.pbrIdx].meshBuffer = Buffer::createMeshBuffer(
        config,
        vertData.data(),
        nullptr
    );

    int flags = HAS_ALBEDO_TEX | HAS_NORMAL_TEX | HAS_METALLIC_TEX;
    m_pbrRenderables[obj.pbrIdx].material.ubo = {
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

    m_pbrRenderables[obj.pbrIdx].material.albedoTexture.loadTexture(std::string(TEXTURE_DIR) + "pbr_earth/albedo_map.jpg", 1);
    m_pbrRenderables[obj.pbrIdx].material.normalTexture.loadTexture(std::string(TEXTURE_DIR) + "pbr_earth/normal_map.jpg", 2);
    m_pbrRenderables[obj.pbrIdx].material.metallicTexture.loadTexture(std::string(TEXTURE_DIR) + "pbr_earth/metal_map.jpg", 4);
    //

    m_pbrRenderables[obj.pbrIdx].transform = {
        .m_matrix = glm::mat4(1.0f),
        .pos = {0.0f, 0.0f, 0.0f},
        .rot = {0.0f, 0.0f, 0.0f},
        .scale = {1.0f, 1.0f, 1.0f}
    };
    m_pbrRenderables[obj.pbrIdx].transform.calcMatrix();
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

void Scene::AddBarelObj() {
    if (m_pbrRenderables.size() <= m_pbrCount) {
        m_pbrRenderables.resize(m_pbrCount + 1);
    }

    Obj obj;
    obj.idx = m_objCount++;
    obj.pbrIdx = m_pbrCount++;
    m_objNames.push_back("Barel " + std::to_string(obj.idx + 1));
    obj.type = RenderType::PBR;
    m_objects.push_back(obj);

    Model barelModel;
    barelModel.loadFromFile(std::string(MODEL_DIR) + "barrel.obj");
    m_models.push_back(barelModel);

    std::vector<DummyVert> vertData = getDummyVerts(m_models[obj.idx].getVertices());
    VAOConfig config = createPBRConfig(obj.idx);
    m_pbrRenderables[obj.pbrIdx].meshBuffer = Buffer::createMeshBuffer(
        config,
        vertData.data(),
        nullptr
    );

    int flags = HAS_ALBEDO_TEX | HAS_NORMAL_TEX | HAS_ROUGHNESS_TEX | HAS_METALLIC_TEX;
    m_pbrRenderables[obj.pbrIdx].material.ubo = {
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

    m_pbrRenderables[obj.pbrIdx].material.albedoTexture.loadTexture(std::string(TEXTURE_DIR) + "pbr_barel/albedo.png", 1);
    m_pbrRenderables[obj.pbrIdx].material.normalTexture.loadTexture(std::string(TEXTURE_DIR) + "pbr_barel/normal.png", 2);
    m_pbrRenderables[obj.pbrIdx].material.roughnessTexture.loadTexture(std::string(TEXTURE_DIR) + "pbr_barel/roughness.png", 3);
    m_pbrRenderables[obj.pbrIdx].material.metallicTexture.loadTexture(std::string(TEXTURE_DIR) + "pbr_barel/metallic.png", 4);
    

    m_pbrRenderables[obj.pbrIdx].transform = {
        .m_matrix = glm::mat4(1.0f),
        .pos = {0.0f, 0.0f, 0.0f},
        .rot = {0.0f, 0.0f, 0.0f},
        .scale = {1.0f, 1.0f, 1.0f}
    };
    m_pbrRenderables[obj.pbrIdx].transform.calcMatrix();
}

void Scene::AddLight() {
    Light mainLight;
    
    mainLight.pos = {0.0f, 1.0f, 3.0f};
    mainLight.color = {1.0f, 1.0f, 1.0f};

    glGenVertexArrays(1, &mainLight.dummyVAO);
    m_renderInfo.lights.push_back(mainLight);

}

void Scene::AddPBRCubeObj() {
    if (m_pbrRenderables.size() <= m_pbrCount) {
        m_pbrRenderables.resize(m_pbrCount + 1);
    }

    Obj obj;
    obj.idx = m_objCount++;
    obj.pbrIdx = m_pbrCount++;
    m_objNames.push_back("PBR Cube " + std::to_string(obj.idx + 1));
    obj.type = RenderType::PBR;
    m_objects.push_back(obj);

    Model cubeModel;
    cubeModel.CubeModelUnique();
    m_models.push_back(cubeModel);

    std::vector<DummyVert> vertData = getDummyVerts(m_models[obj.idx].getVertices());

    VAOConfig config = createPBRConfig(obj.idx);
    m_pbrRenderables[obj.pbrIdx].meshBuffer = Buffer::createMeshBuffer(
        config,
        vertData.data(),
        nullptr
    );

    int flags = 0;
    // set bit 1, bit 2, bit 3, and bit 5
    flags |= HAS_ALBEDO_TEX | HAS_NORMAL_TEX | HAS_ROUGHNESS_TEX;
    //flags |= HAS_ALBEDO_TEX | HAS_NORMAL_TEX | HAS_ROUGHNESS_TEX | HAS_AO_TEX;
    //flags |= HAS_ALBEDO_TEX | HAS_NORMAL_TEX | HAS_ROUGHNESS_TEX | HAS_METALLIC_TEX;

    m_pbrRenderables[obj.pbrIdx].material.ubo = {
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

    //m_pbrRenderables[obj.pbrIdx].material.albedoTexture.loadTexture(std::string(TEXTURE_DIR) + "pbr_wood/albedo_map.jpg", 1);
    //m_pbrRenderables[obj.pbrIdx].material.normalTexture.loadTexture(std::string(TEXTURE_DIR) + "pbr_wood/normal_map.jpg", 2);
    //m_pbrRenderables[obj.pbrIdx].material.roughnessTexture.loadTexture(std::string(TEXTURE_DIR) + "pbr_wood/roughness_map.jpg", 3);
    //m_pbrRenderables[obj.pbrIdx].material.aoTexture.loadTexture(std::string(TEXTURE_DIR) + "pbr_wood/ao_map.jpg", 5);

    //m_pbrRenderables[obj.pbrIdx].material.albedoTexture.loadTexture(std::string(TEXTURE_DIR) + "pbr_metal/albedo_map.jpg", 1);
    //m_pbrRenderables[obj.pbrIdx].material.normalTexture.loadTexture(std::string(TEXTURE_DIR) + "pbr_metal/normal_map.jpg", 2);
    //m_pbrRenderables[obj.pbrIdx].material.roughnessTexture.loadTexture(std::string(TEXTURE_DIR) + "pbr_metal/roughness_map.jpg", 3);
    //m_pbrRenderables[obj.pbrIdx].material.metallicTexture.loadTexture(std::string(TEXTURE_DIR) + "pbr_metal/metal_map.jpg", 4);

    m_pbrRenderables[obj.pbrIdx].material.albedoTexture.loadTexture(std::string(TEXTURE_DIR) + "pbr_tiles/albedo_map.jpg", 1);
    m_pbrRenderables[obj.pbrIdx].material.normalTexture.loadTexture(std::string(TEXTURE_DIR) + "pbr_tiles/normal_map.jpg", 2);
    m_pbrRenderables[obj.pbrIdx].material.roughnessTexture.loadTexture(std::string(TEXTURE_DIR) + "pbr_tiles/roughness_map.jpg", 3);

    m_pbrRenderables[obj.pbrIdx].transform = {
        .m_matrix = glm::mat4(1.0f),
        .pos = {0.0f, 0.0f, 0.0f},
        .rot = {0.0f, 0.0f, 0.0f},
        .scale = {1.0f, 1.0f, 1.0f}
    };

    m_pbrRenderables[obj.pbrIdx].transform.calcMatrix();

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
    Obj& obj = m_objects[idx];
    if (obj.type == RenderType::PBR) {
        m_pbrRenderables[obj.pbrIdx].meshBuffer.cleanup();
        m_pbrRenderables.erase(m_pbrRenderables.begin() + obj.pbrIdx);
        m_pbrCount--;
        // delete the model
        m_models.erase(m_models.begin() + obj.idx);
        // delete the object name
        m_objNames.erase(m_objNames.begin() + obj.idx);
        // update the indices of the objects
        for (size_t i = idx; i < m_objects.size(); ++i) {
            if (m_objects[i].type != RenderType::PBR) continue;
            m_objects[i].pbrIdx--;
        }
    }

    // delete the object
    m_objects.erase(m_objects.begin() + idx);
    for (size_t i = idx; i < m_objects.size(); ++i) {
        m_objects[i].idx--;
    }
    m_objCount--;
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