#include "ShaderProgram.h"

ShaderProgram::ShaderProgram() : m_ProgramId(0) {}

ShaderProgram::~ShaderProgram() {
    Terminate();
}

bool ShaderProgram::Create(const char* vs, const char* fs) {
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vs);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fs);
    m_ProgramId = glCreateProgram();
    glAttachShader(m_ProgramId, vertexShader);
    glAttachShader(m_ProgramId, fragmentShader);
    glLinkProgram(m_ProgramId);
    return true;
}

void ShaderProgram::Use() {
    if (m_ProgramId != 0) {
        glUseProgram(m_ProgramId);
    }
}

void ShaderProgram::Terminate() {
    if (m_ProgramId != 0) {
        glDeleteProgram(m_ProgramId);
        m_ProgramId = 0;
    }
}

GLuint ShaderProgram::CompileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    return shader;
}
