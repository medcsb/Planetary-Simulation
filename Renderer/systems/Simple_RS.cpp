#include "Simple_RS.hpp"

#include <glm/gtc/type_ptr.hpp>

Simple_RS::Simple_RS() {}
Simple_RS::~Simple_RS() {}

void Simple_RS::cleanup() {
    glDeleteBuffers(1, &m_materialUBO);
    m_materialUBO = 0;
}

void Simple_RS::init(GLuint shaderProgram) {
    m_shaderProgram = shaderProgram;

    glGenBuffers(1, &m_materialUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_materialUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SimpleUBO), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_materialUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Simple_RS::render(RenderInfo& renderInfo) {
    glUseProgram(m_shaderProgram);
    glm::vec3 cameraPos = glm::vec3(glm::inverse(renderInfo.viewMatrix)[3]);
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(renderInfo.viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(renderInfo.projectionMatrix));
    glUniform3fv(glGetUniformLocation(m_shaderProgram, "lightPos"), 1, glm::value_ptr(renderInfo.lights[0].pos));
    glUniform3fv(glGetUniformLocation(m_shaderProgram, "lightColor"), 1, glm::value_ptr(renderInfo.lights[0].color));
    glUniform3fv(glGetUniformLocation(m_shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPos));
    for (const auto& renderable : *m_renderableQueue) {
        glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(renderable.transform.m_matrix));

        glActiveTexture(GL_TEXTURE0 + renderable.material.albedoTexture.getTextureUnit());
        glBindTexture(GL_TEXTURE_2D, renderable.material.albedoTexture.getTextureId());
        glUniform1i(glGetUniformLocation(m_shaderProgram, "tex0"), renderable.material.albedoTexture.getTextureUnit());

        glBindBuffer(GL_UNIFORM_BUFFER, m_materialUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SimpleUBO), &renderable.material.ubo);

        glBindTexture(GL_TEXTURE_2D, renderable.material.albedoTexture.getTextureId());
        
        renderable.meshBuffer.bind();
        renderable.meshBuffer.draw();
    }
}