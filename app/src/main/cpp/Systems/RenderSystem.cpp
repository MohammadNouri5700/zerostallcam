#include "RenderSystem.h"
#include "ShaderManager.h"
#include <GLES2/gl2ext.h>
#include <chrono>
#include <android/log.h>
#include <android/trace.h>

#define LOG_TAG "ZeroStallRenderer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

#ifndef GL_TIME_ELAPSED_EXT
#define GL_TIME_ELAPSED_EXT 0x88BF
#endif

namespace ecs {

GLuint RenderSystem::mTransformUbo = 0;
GLuint RenderSystem::mTimestampUbo = 0;
GLuint RenderSystem::mDummyVao = 0;

void RenderSystem::Init(GraphicsComponent& graphics, AAssetManager* assetManager) {
    graphics.renderProgram = ShaderManager::CreateGraphicsProgramFromAssets(assetManager, "shaders/scene.vert", "shaders/scene.frag");

    GLuint transformIdx = glGetUniformBlockIndex(graphics.renderProgram, "TransformBlock");
    glUniformBlockBinding(graphics.renderProgram, transformIdx, 1);

    GLuint timestampIdx = glGetUniformBlockIndex(graphics.renderProgram, "TimestampBlock");
    glUniformBlockBinding(graphics.renderProgram, timestampIdx, 2);

    glGenTextures(1, &graphics.cameraTextureId);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, graphics.cameraTextureId);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenBuffers(1, &mTransformUbo);
    glBindBuffer(GL_UNIFORM_BUFFER, mTransformUbo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(TransformComponent), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, mTransformUbo);

    glGenBuffers(1, &mTimestampUbo);
    glBindBuffer(GL_UNIFORM_BUFFER, mTimestampUbo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(TimestampComponent), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, mTimestampUbo);

    glGenVertexArrays(1, &mDummyVao);
    glBindVertexArray(mDummyVao);

#ifdef MEASUREMENT_ENABLED
    if (graphics.gpuTimerSupported) {
        glGenQueries(1, &graphics.gpuTimerQuery);
    }
    graphics.lastLogTime = std::chrono::steady_clock::now();
#endif
}

void RenderSystem::UpdateCameraBuffer(GraphicsComponent& graphics, AHardwareBuffer* buffer) {
    auto it = graphics.eglImageCache.find(buffer);
    if (it != graphics.eglImageCache.end()) {
        graphics.eglImage = it->second;
    } else {
        EGLClientBuffer clientBuffer = graphics.eglGetNativeClientBufferANDROID(buffer);
        graphics.eglImage = graphics.eglCreateImageKHR(graphics.display, EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_ANDROID, clientBuffer, nullptr);
        graphics.eglImageCache[buffer] = graphics.eglImage;
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, graphics.cameraTextureId);
    graphics.glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, graphics.eglImage);
}

void RenderSystem::SetFontAtlas(GraphicsComponent& graphics, int width, int height, void* pixels) {
    if (graphics.fontAtlasTextureId != 0) glDeleteTextures(1, &graphics.fontAtlasTextureId);
    glGenTextures(1, &graphics.fontAtlasTextureId);
    glBindTexture(GL_TEXTURE_2D, graphics.fontAtlasTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

void RenderSystem::DrawFrame(GraphicsComponent& graphics, const TransformComponent& transform, const TimestampComponent& timestamp) {
#ifdef MEASUREMENT_ENABLED
    ATrace_beginSection("ZeroStall_DrawFrame");
    if (graphics.gpuTimerSupported) {
        glBeginQuery(GL_TIME_ELAPSED_EXT, graphics.gpuTimerQuery);
    }
#endif

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(graphics.renderProgram);

#ifdef MEASUREMENT_ENABLED
    ATrace_beginSection("ZeroStall_UboUpdate");
#endif
    // Update Transform UBO
    glBindBuffer(GL_UNIFORM_BUFFER, mTransformUbo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(TransformComponent), &transform);

    // Update Timestamp UBO
    glBindBuffer(GL_UNIFORM_BUFFER, mTimestampUbo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(TimestampComponent), &timestamp);
#ifdef MEASUREMENT_ENABLED
    ATrace_endSection();
#endif

#ifdef MEASUREMENT_ENABLED
    ATrace_beginSection("ZeroStall_TextureBind");
#endif
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, graphics.cameraTextureId);
    glUniform1i(glGetUniformLocation(graphics.renderProgram, "sCamera"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, graphics.fontAtlasTextureId);
    glUniform1i(glGetUniformLocation(graphics.renderProgram, "sFont"), 1);
#ifdef MEASUREMENT_ENABLED
    ATrace_endSection();
#endif

    // Single procedural draw call for all 54 vertices (6 BG + 48 Text)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_TRIANGLES, 0, 54);

    graphics.frameCount++;

#ifdef MEASUREMENT_ENABLED
    if (graphics.gpuTimerSupported) {
        glEndQuery(GL_TIME_ELAPSED_EXT);
    }
    ATrace_endSection();
#endif
}

void RenderSystem::Deinit(GraphicsComponent& graphics) {
    ShaderManager::TerminateProgram(graphics.renderProgram);
    if (graphics.cameraTextureId != 0) glDeleteTextures(1, &graphics.cameraTextureId);
    if (graphics.fontAtlasTextureId != 0) glDeleteTextures(1, &graphics.fontAtlasTextureId);
    if (mTransformUbo != 0) glDeleteBuffers(1, &mTransformUbo);
    if (mTimestampUbo != 0) glDeleteBuffers(1, &mTimestampUbo);
    if (mDummyVao != 0) glDeleteVertexArrays(1, &mDummyVao);

#ifdef MEASUREMENT_ENABLED
    if (graphics.gpuTimerQuery != 0) glDeleteQueries(1, &graphics.gpuTimerQuery);
#endif

    for (auto const& [buffer, image] : graphics.eglImageCache) {
        graphics.eglDestroyImageKHR(graphics.display, image);
    }
    graphics.eglImageCache.clear();
    graphics.eglImage = EGL_NO_IMAGE_KHR;
}

} // namespace ecs
