#ifndef MESH_H
#define MESH_H

#define _USE_MATH_DEFINES

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <vector>
#include <memory>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <memory>

class Mesh {
    public:
        void init();
        void render(GLuint g_program, GLuint textureID);
        static std::shared_ptr<Mesh> genSphere(float radius, const size_t resolution);
        void setModelMatrix(const glm::mat4 &modelMatrix);

        // Animation
        void orbitAndRotate(const glm::vec3 orbitCenter, const float orbitRadius, const glm::vec3 orbitAxis, float orbitAngle, const glm::vec3 rotationAxis, float rotationAngle);

        glm::mat4 getModelMatrix() {
            return modelMatrix;
        }
        
        glm::vec3 getPosition() {
            return position;
        }

    private:
        std::vector<float> m_vertexPositions;
        std::vector<unsigned int> m_triangleIndices;
        std::vector<float> m_vertexNormals;
        std::vector<float> m_vertexTecCoords;
        GLuint m_vao = 0;
        GLuint m_posVbo = 0;
        GLuint m_ibo = 0;
        GLuint m_normalVbo = 0;
        GLuint m_texCoordVbo = 0;
        GLint m_modelMatLoc = 0;

        glm::mat4 modelMatrix = glm::mat4(1.0f);

        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

        void setVertexIndices(const size_t sectorCount, const size_t stackCount);
};

#endif // MESH_H
