#include "ShaderManager.h"
#include <android/log.h>
#include <vector>

#define LOG_TAG "ZeroStallShaderManager"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace ecs {

GLuint ShaderManager::CreateGraphicsProgramFromAssets(AAssetManager* assetManager, const char* vsPath, const char* fsPath) {
    std::string vsSource = LoadAssetAsString(assetManager, vsPath);
    std::string fsSource = LoadAssetAsString(assetManager, fsPath);
    if (vsSource.empty() || fsSource.empty()) return 0;
    return CreateGraphicsProgram(vsSource.c_str(), fsSource.c_str());
}

GLuint ShaderManager::CreateGraphicsProgram(const char* vs, const char* fs) {
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vs);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fs);
    if (vertexShader == 0 || fragmentShader == 0) return 0;

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint linkStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        LOGE("Program Link Error: %s", log);
        glDeleteProgram(program);
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

GLuint ShaderManager::CreateComputeProgram(const char* cs) {
    GLuint computeShader = CompileShader(GL_COMPUTE_SHADER, cs);
    if (computeShader == 0) return 0;

    GLuint program = glCreateProgram();
    glAttachShader(program, computeShader);
    glLinkProgram(program);

    GLint linkStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        LOGE("Compute Program Link Error: %s", log);
        glDeleteProgram(program);
        return 0;
    }

    glDeleteShader(computeShader);
    return program;
}

void ShaderManager::TerminateProgram(GLuint program) {
    if (program != 0) glDeleteProgram(program);
}

GLuint ShaderManager::CompileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus != GL_TRUE) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        LOGE("Shader Compile Error (%d): %s", type, log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

std::string ShaderManager::LoadAssetAsString(AAssetManager* assetManager, const char* path) {
    AAsset* asset = AAssetManager_open(assetManager, path, AASSET_MODE_BUFFER);
    if (!asset) {
        LOGE("Failed to open asset: %s", path);
        return "";
    }
    off_t length = AAsset_getLength(asset);
    std::string content(length, '\0');
    AAsset_read(asset, &content[0], (size_t)length);
    AAsset_close(asset);
    return content;
}

} // namespace ecs
