#ifndef ZEROSTALLCAM_SYSTEMS_SHADER_MANAGER_H
#define ZEROSTALLCAM_SYSTEMS_SHADER_MANAGER_H

#include <GLES3/gl31.h>

namespace ecs {

class ShaderManager {
public:
    static GLuint CreateGraphicsProgram(const char* vs, const char* fs);
    static GLuint CreateComputeProgram(const char* cs);
    static void TerminateProgram(GLuint program);

private:
    static GLuint CompileShader(GLenum type, const char* source);
};

} // namespace ecs

#endif //ZEROSTALLCAM_SYSTEMS_SHADER_MANAGER_H
