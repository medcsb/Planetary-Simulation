#pragma once

#include "glad/glad.h"

#include <string>

class Shader {
private:
    GLuint m_vertId, m_fragId, m_computeId;
    GLuint m_id;

    const std::string m_vertPath, m_fragPath, m_computePath;

public:
    Shader(const std::string& vertPath, const std::string& fragPath);
    ~Shader();

    void init();
    void use();
    void reload();

    void cleanup();

    GLuint getProgramId() const { return m_id; }

private:
    void createProg();
    void compileShader(GLuint& id, GLuint type, const std::string& code);
    void checkCompileErrors(GLuint id, GLuint type);
};