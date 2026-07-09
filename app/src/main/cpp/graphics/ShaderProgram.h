#ifndef ZEROSTALLCAM_SHADERPROGRAM_H
#define ZEROSTALLCAM_SHADERPROGRAM_H

#include <GLES3/gl3.h>

class ShaderProgram {
public:
    ShaderProgram();
    ~ShaderProgram();

    bool Create(const char* vs, const char* fs);
    void Use();
    void Terminate();
    GLuint GetId() const { return m_ProgramId; }

private:
    GLuint CompileShader(GLenum type, const char* source);
    GLuint m_ProgramId;
};

#endif //ZEROSTALLCAM_SHADERPROGRAM_H
