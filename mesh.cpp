#include "mesh.h"

void Mesh::init(){
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    size_t posBufferSize = sizeof(float)*m_vertexPositions.size();
    glGenBuffers(1, &m_posVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_posVbo);
    glBufferData(GL_ARRAY_BUFFER, posBufferSize, m_vertexPositions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);

    size_t normalBufferSize = sizeof(float)*m_vertexNormals.size();
    glGenBuffers(1, &m_normalVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_normalVbo);
    glBufferData(GL_ARRAY_BUFFER, normalBufferSize, m_vertexNormals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);

    size_t indexBufferSize = sizeof(unsigned int)*m_triangleIndices.size();
    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, m_triangleIndices.data(), GL_STATIC_DRAW);

    size_t texCoordBufferSize = sizeof(float)*m_vertexTecCoords.size();
    glGenBuffers(1, &m_texCoordVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_texCoordVbo);
    glBufferData(GL_ARRAY_BUFFER, texCoordBufferSize, m_vertexTecCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), 0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Mesh::setModelMatrix(const glm::mat4 &modelMatrix) {
    this->modelMatrix = modelMatrix;
    this->position = glm::vec3(modelMatrix[3]);
}

void Mesh::render(GLuint g_program, GLuint textureID) {
    m_modelMatLoc = glGetUniformLocation(g_program, "modelMat");
    glUniformMatrix4fv(m_modelMatLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    if (textureID != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_triangleIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

std::shared_ptr<Mesh> Mesh::genSphere(float radius, const size_t resolution) {
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal

    const size_t sectorCount = resolution;
    const size_t stackCount = resolution/2;

    const float sectorStep = 2*M_PI / sectorCount;
    const float stackStep = M_PI / stackCount;

    float sectorAngle, stackAngle;

    for (size_t i = 0; i <= stackCount; ++i) {
        stackAngle = M_PI/2 - i*stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);           // r * cos(u)
        z = radius * sinf(stackAngle);            // r * sin(u)

        for (size_t j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            mesh->m_vertexPositions.push_back(x);
            mesh->m_vertexPositions.push_back(z);
            mesh->m_vertexPositions.push_back(y);

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            mesh->m_vertexNormals.push_back(nx);
            mesh->m_vertexNormals.push_back(nz);
            mesh->m_vertexNormals.push_back(ny);

            // vertex tex coord (s, t) range between [0, 1]
            mesh->m_vertexTecCoords.push_back((float)j / sectorCount);
            mesh->m_vertexTecCoords.push_back((float)i / stackCount);
        }
    }

    mesh->setVertexIndices(sectorCount, stackCount);

    return mesh;
}

void Mesh::setVertexIndices(const size_t sectorCount, const size_t stackCount) {
    int k1, k2;

    for(size_t i = 0; i < stackCount; ++i) {
        k1 = i * (sectorCount + 1);
        k2 = k1 + sectorCount + 1;

        for(size_t j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if(i != 0) {
                m_triangleIndices.push_back(k1);
                m_triangleIndices.push_back(k2);
                m_triangleIndices.push_back(k1 + 1);
            }

            if(i != (stackCount - 1)) {
                m_triangleIndices.push_back(k1 + 1);
                m_triangleIndices.push_back(k2);
                m_triangleIndices.push_back(k2 + 1);
            }
        }
    }
}

void Mesh::orbitAndRotate(const glm::vec3 orbitCenter, const float orbitRadius, const glm::vec3 orbitAxis, float orbitAngle, const glm::vec3 rotationAxis, float rotationAngle) {
    glm::vec3 normalizedOrbitAxis = glm::normalize(orbitAxis);
    glm::vec3 normalizedRotationAxis = glm::normalize(rotationAxis);

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, orbitCenter);
    model = glm::rotate(model, glm::radians(orbitAngle), normalizedOrbitAxis);

    model = glm::translate(model, glm::vec3(orbitRadius, 0.0f, 0.0f));

    glm::quat rotationQuat = glm::angleAxis(glm::radians(rotationAngle), normalizedRotationAxis);
    glm::mat4 rotationMatrix = glm::mat4_cast(rotationQuat);

    model = model * rotationMatrix;

    setModelMatrix(model);
}
    