#ifndef ZEROSTALLCAM_SYSTEMS_SHADER_MANAGER_H
#define ZEROSTALLCAM_SYSTEMS_SHADER_MANAGER_H

#include <GLES3/gl31.h>
#include <android/asset_manager.h>
#include <string>

namespace ecs {

class ShaderManager {
public:
    static GLuint CreateGraphicsProgramFromAssets(AAssetManager* assetManager, const char* vsPath, const char* fsPath);
    static GLuint CreateGraphicsProgram(const char* vs, const char* fs);
    static GLuint CreateComputeProgram(const char* cs);
    static void TerminateProgram(GLuint program);

private:
    static std::string LoadAssetAsString(AAssetManager* assetManager, const char* path);
    static GLuint CompileShader(GLenum type, const char* source);
};

} // namespace ecs

#endif //ZEROSTALLCAM_SYSTEMS_SHADER_MANAGER_H
