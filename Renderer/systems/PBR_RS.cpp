#include "PBR_RS.hpp"

#include <glm/gtc/type_ptr.hpp>

PBR_RS::PBR_RS() {}
PBR_RS::~PBR_RS() {}

void PBR_RS::cleanup() {
    glDeleteBuffers(1, &m_materialUBO);
    m_materialUBO = 0;
}

void PBR_RS::init(GLuint shaderProgram) {
    m_shaderProgram = shaderProgram;

    glGenBuffers(1, &m_materialUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_materialUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PBR_UBO), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_materialUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 1);
}

void PBR_RS::render(RenderInfo& renderInfo) {
    glUseProgram(m_shaderProgram);
    glm::vec3 cameraPos = glm::vec3(glm::inverse(renderInfo.viewMatrix)[3]);
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(renderInfo.viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(renderInfo.projectionMatrix));
    glUniform3fv(glGetUniformLocation(m_shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPos));
    glUniform3fv(glGetUniformLocation(m_shaderProgram, "lightPos"), 1, glm::value_ptr(renderInfo.lights[0].pos));
    glUniform3fv(glGetUniformLocation(m_shaderProgram, "lightColor"), 1, glm::value_ptr(renderInfo.lights[0].color));

    for (const auto& renderable : *m_renderableQueue) {
        glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(renderable.transform.m_matrix));

        // bind textures
        // albedo Map
        glActiveTexture(GL_TEXTURE0 + renderable.material.albedoTexture.getTextureUnit());
        glBindTexture(GL_TEXTURE_2D, renderable.material.albedoTexture.getTextureId());
        glUniform1i(glGetUniformLocation(m_shaderProgram, "albedoMap"), renderable.material.albedoTexture.getTextureUnit());

        // normal Map
        glActiveTexture(GL_TEXTURE0 + renderable.material.normalTexture.getTextureUnit());
        glBindTexture(GL_TEXTURE_2D, renderable.material.normalTexture.getTextureId());
        glUniform1i(glGetUniformLocation(m_shaderProgram, "normalMap"), renderable.material.normalTexture.getTextureUnit());

        // metallic Map
        glActiveTexture(GL_TEXTURE0 + renderable.material.metallicTexture.getTextureUnit());
        glBindTexture(GL_TEXTURE_2D, renderable.material.metallicTexture.getTextureId());
        glUniform1i(glGetUniformLocation(m_shaderProgram, "metallicMap"), renderable.material.metallicTexture.getTextureUnit());

        // roughness Map
        glActiveTexture(GL_TEXTURE0 + renderable.material.roughnessTexture.getTextureUnit());
        glBindTexture(GL_TEXTURE_2D, renderable.material.roughnessTexture.getTextureId());
        glUniform1i(glGetUniformLocation(m_shaderProgram, "roughnessMap"), renderable.material.roughnessTexture.getTextureUnit());
        
        // ambient occlusion Map
        glActiveTexture(GL_TEXTURE0 + renderable.material.aoTexture.getTextureUnit());
        glBindTexture(GL_TEXTURE_2D, renderable.material.aoTexture.getTextureId());
        glUniform1i(glGetUniformLocation(m_shaderProgram, "aoMap"), renderable.material.aoTexture.getTextureUnit());

        // Update the material UBO
        glBindBuffer(GL_UNIFORM_BUFFER, m_materialUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PBR_UBO), &renderable.material.ubo);

        glBindTexture(GL_TEXTURE_2D, renderable.material.albedoTexture.getTextureId());
        
        renderable.meshBuffer.bind();
        renderable.meshBuffer.draw();
    }
}