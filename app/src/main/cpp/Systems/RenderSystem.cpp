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

const char* VS = R"(#version 300 es
layout(std140) uniform TransformBlock { vec4 s_c; vec4 rot_p; } transform;
layout(std140) uniform TimestampBlock { vec4 timeData; } timestamp;
out vec2 vTexCoord; out float vType;
const float kTextPosX = -0.99; const float kTextPosY = 0.99;
const float kTextCharWidth = 0.06; const float kTextCharHeight = 0.05;
void main() {
    int idx = gl_VertexID; vec4 pos; vec2 tex; float type;
    float sw = transform.s_c.x; float sh = transform.s_c.y;
    float cw = transform.s_c.z; float ch = transform.s_c.w;
    float rot = transform.rot_p.x;
    if (idx < 6) {
        type = 0.0;
        bool isRot = (rot == 90.0 || rot == 270.0);
        float acW = isRot ? ch : cw;
        float acH = isRot ? cw : ch;
        float sA = sw / sh;
        float cA = acW / acH;
        float cL = 0.0, cR = 1.0, cT = 0.0, cB = 1.0;
        if (sA > cA) { float r = cA / sA; float a = (1.0 - r) / 2.0; cT = a; cB = 1.0 - a; }
        else { float r = sA / cA; float a = (1.0 - r) / 2.0; cL = a; cR = 1.0 - a; }
        float uvs[8];
        if (rot == 90.0) { uvs[0] = cL; uvs[1] = cB; uvs[2] = cR; uvs[3] = cB; uvs[4] = cL; uvs[5] = cT; uvs[6] = cR; uvs[7] = cT; }
        else { uvs[0] = cL; uvs[1] = cT; uvs[2] = cL; uvs[3] = cB; uvs[4] = cR; uvs[5] = cT; uvs[6] = cR; uvs[7] = cB; }
        if (idx == 0) { pos = vec4(-1.0,  1.0, 0.0, 1.0); tex = vec2(uvs[0], uvs[1]); }
        else if (idx == 1) { pos = vec4(-1.0, -1.0, 0.0, 1.0); tex = vec2(uvs[2], uvs[3]); }
        else if (idx == 2) { pos = vec4( 1.0,  1.0, 0.0, 1.0); tex = vec2(uvs[4], uvs[5]); }
        else if (idx == 3) { pos = vec4(-1.0, -1.0, 0.0, 1.0); tex = vec2(uvs[2], uvs[3]); }
        else if (idx == 4) { pos = vec4( 1.0, -1.0, 0.0, 1.0); tex = vec2(uvs[6], uvs[7]); }
        else if (idx == 5) { pos = vec4( 1.0,  1.0, 0.0, 1.0); tex = vec2(uvs[4], uvs[5]); }
    } else {
        type = 1.0; int vIdx = idx - 6; int cIdx = vIdx / 6; int tIdx = vIdx % 6;
        int code;
        float epoch = timestamp.timeData.x;
        int h = int(mod(epoch / 3600.0, 24.0));
        int m = int(mod(epoch / 60.0, 60.0));
        int s = int(mod(epoch, 60.0));
        if (cIdx == 0) code = h / 10; else if (cIdx == 1) code = h % 10;
        else if (cIdx == 2) code = 10; else if (cIdx == 3) code = m / 10;
        else if (cIdx == 4) code = m % 10; else if (cIdx == 5) code = 10;
        else if (cIdx == 6) code = s / 10; else if (cIdx == 7) code = s % 10;
        float uS = float(code) / 11.0; float uE = float(code + 1) / 11.0;
        float xP = kTextPosX + float(cIdx) * kTextCharWidth;
        if (tIdx == 0) { pos = vec4(xP, kTextPosY, 0.0, 1.0); tex = vec2(uS, 0.0); }
        else if (tIdx == 1) { pos = vec4(xP, kTextPosY - kTextCharHeight, 0.0, 1.0); tex = vec2(uS, 1.0); }
        else if (tIdx == 2) { pos = vec4(xP + kTextCharWidth, kTextPosY, 0.0, 1.0); tex = vec2(uE, 0.0); }
        else if (tIdx == 3) { pos = vec4(xP, kTextPosY - kTextCharHeight, 0.0, 1.0); tex = vec2(uS, 1.0); }
        else if (tIdx == 4) { pos = vec4(xP + kTextCharWidth, kTextPosY - kTextCharHeight, 0.0, 1.0); tex = vec2(uE, 1.0); }
        else if (tIdx == 5) { pos = vec4(xP + kTextCharWidth, kTextPosY, 0.0, 1.0); tex = vec2(uE, 0.0); }
    }
    gl_Position = pos; vTexCoord = tex; vType = type;
})";

const char* FS = R"(#version 300 es
#extension GL_OES_EGL_image_external_essl3 : require
precision mediump float;
uniform samplerExternalOES sCamera;
uniform sampler2D sFont;
in vec2 vTexCoord; in float vType;
out vec4 fragColor;
void main() {
    if (vType < 0.5) fragColor = texture(sCamera, vTexCoord);
    else {
        vec4 fontColor = texture(sFont, vTexCoord);
        fragColor = vec4(1.0, 1.0, 0.0, fontColor.a);
    }
})";

void RenderSystem::Init(GraphicsComponent& graphics) {
    graphics.renderProgram = ShaderManager::CreateGraphicsProgram(VS, FS);

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
    glGenQueries(1, &graphics.gpuTimerQuery);
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
    glBeginQuery(GL_TIME_ELAPSED_EXT, graphics.gpuTimerQuery);
#endif

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(graphics.renderProgram);

    // Update Transform UBO
    glBindBuffer(GL_UNIFORM_BUFFER, mTransformUbo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(TransformComponent), &transform);

    // Update Timestamp UBO
    glBindBuffer(GL_UNIFORM_BUFFER, mTimestampUbo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(TimestampComponent), &timestamp);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, graphics.cameraTextureId);
    glUniform1i(glGetUniformLocation(graphics.renderProgram, "sCamera"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, graphics.fontAtlasTextureId);
    glUniform1i(glGetUniformLocation(graphics.renderProgram, "sFont"), 1);

    // Single procedural draw call for all 54 vertices (6 BG + 48 Text)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_TRIANGLES, 0, 54);

    graphics.frameCount++;

#ifdef MEASUREMENT_ENABLED
    glEndQuery(GL_TIME_ELAPSED_EXT);
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
