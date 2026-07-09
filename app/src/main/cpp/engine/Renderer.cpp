#include "Renderer.h"
#include <GLES2/gl2ext.h>
#include <chrono>
#include <android/trace.h>
#include <android/log.h>

#define LOG_TAG "ZeroStallRenderer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

const char* VS = R"(#version 300 es
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in float aType;
out vec2 vTexCoord;
out float vType;
void main() {
    gl_Position = aPosition;
    vTexCoord = aTexCoord;
    vType = aType;
})";

const char* FS = R"(#version 300 es
#extension GL_OES_EGL_image_external_essl3 : require
precision mediump float;
uniform samplerExternalOES sCamera;
uniform sampler2D sFont;
in vec2 vTexCoord;
in float vType;
out vec4 fragColor;
void main() {
    if (vType < 0.5) {
        fragColor = texture(sCamera, vTexCoord);
    } else {
        vec4 fontColor = texture(sFont, vTexCoord);
        fragColor = vec4(1.0, 1.0, 0.0, fontColor.a);
    }
})";

Renderer::Renderer() :
    m_CameraTextureId(0), m_FontAtlasTextureId(0), m_EglImage(EGL_NO_IMAGE_KHR),
    m_CurrentBuffer(nullptr),
    eglCreateImageKHR(nullptr), eglDestroyImageKHR(nullptr),
    glEGLImageTargetTexture2DOES(nullptr), eglGetNativeClientBufferANDROID(nullptr),
    m_FrameCount(0), m_WindowWidth(0), m_WindowHeight(0) {

    for (int i = 0; i < 8; i++) m_BakedUVs[i] = 0.0f;

    m_Egl = std::make_unique<EglManager>();
    m_Shader = std::make_unique<ShaderProgram>();
    m_Geometry = std::make_unique<SceneGeometry>();
}

Renderer::~Renderer() {
    if (m_EglImage != EGL_NO_IMAGE_KHR) {
        eglDestroyImageKHR(m_Egl->GetDisplay(), m_EglImage);
    }
}

void Renderer::Init(ANativeWindow* window) {
    m_Egl->Init(window);
    m_WindowWidth = ANativeWindow_getWidth(window);
    m_WindowHeight = ANativeWindow_getHeight(window);

    eglCreateImageKHR = (PFNEGLCREATEIMAGEKHRPROC)eglGetProcAddress("eglCreateImageKHR");
    eglDestroyImageKHR = (PFNEGLDESTROYIMAGEKHRPROC)eglGetProcAddress("eglDestroyImageKHR");
    glEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)eglGetProcAddress("glEGLImageTargetTexture2DOES");
    eglGetNativeClientBufferANDROID = (PFNEGLGETNATIVECLIENTBUFFERANDROIDPROC)eglGetProcAddress("eglGetNativeClientBufferANDROID");

    m_Shader->Create(VS, FS);

    glGenTextures(1, &m_CameraTextureId);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, m_CameraTextureId);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    m_Geometry->Create();

    // Default Android camera rotation is 90 degrees for back sensor
    CalculatePreBakedUVs(m_WindowWidth, m_WindowHeight, kCameraWidth, kCameraHeight, 90, m_BakedUVs);
}

void Renderer::CreateHDFontAtlas(int width, int height, void* pixels) {
    if (m_FontAtlasTextureId != 0) {
        glDeleteTextures(1, &m_FontAtlasTextureId);
    }
    glGenTextures(1, &m_FontAtlasTextureId);
    glBindTexture(GL_TEXTURE_2D, m_FontAtlasTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

void Renderer::UpdateCameraBuffer(AHardwareBuffer* buffer) {
    if (m_EglImage != EGL_NO_IMAGE_KHR) {
        eglDestroyImageKHR(m_Egl->GetDisplay(), m_EglImage);
    }
    m_CurrentBuffer = buffer;
    EGLClientBuffer clientBuffer = eglGetNativeClientBufferANDROID(buffer);
    m_EglImage = eglCreateImageKHR(m_Egl->GetDisplay(), EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_ANDROID, clientBuffer, nullptr);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, m_CameraTextureId);
    glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, m_EglImage);
}

void Renderer::CalculatePreBakedUVs(int screenW, int screenH, int camW, int camH, int rotationDeg, float* outUVs) {
    bool isRotated = (rotationDeg == 90 || rotationDeg == 270);
    float activeCamW = isRotated ? (float)camH : (float)camW;
    float activeCamH = isRotated ? (float)camW : (float)camH;

    float screenAspect = (float)screenW / (float)screenH;
    float camAspect = activeCamW / activeCamH;

    // Calculate crop percentages (0.0 to 1.0)
    float cropLeft = 0.0f, cropRight = 1.0f;
    float cropTop = 0.0f, cropBottom = 1.0f;

    if (screenAspect > camAspect) {
        // Screen is wider, crop top and bottom
        float ratio = camAspect / screenAspect;
        float cropAmount = (1.0f - ratio) / 2.0f;
        cropTop = cropAmount;
        cropBottom = 1.0f - cropAmount;
    } else {
        // Screen is taller, crop left and right
        float ratio = screenAspect / camAspect;
        float cropAmount = (1.0f - ratio) / 2.0f;
        cropLeft = cropAmount;
        cropRight = 1.0f - cropAmount;
    }

    // Assign the UVs based on sensor rotation (90 degrees corrected for both vertical and horizontal orientation)
    if (rotationDeg == 90) {
        // Vertical mapping: Top to Bottom samples cropLeft to cropRight
        // Horizontal mapping: Left to Right samples cropBottom to cropTop (Flipped to fix left/right movement)
        outUVs[0] = cropLeft;  outUVs[1] = cropBottom;
        outUVs[2] = cropRight; outUVs[3] = cropBottom;
        outUVs[4] = cropLeft;  outUVs[5] = cropTop;
        outUVs[6] = cropRight; outUVs[7] = cropTop;
    } else {
        // Default (0 degrees or unsupported)
        outUVs[0] = cropLeft;  outUVs[1] = cropTop;
        outUVs[2] = cropLeft;  outUVs[3] = cropBottom;
        outUVs[4] = cropRight; outUVs[5] = cropTop;
        outUVs[6] = cropRight; outUVs[7] = cropBottom;
    }
}

void Renderer::DrawFrame() {
#ifdef MEASUREMENT_ENABLED
    auto startTime = std::chrono::steady_clock::now();
    ATrace_beginSection("ZeroStall_DrawFrame");
#endif

    glClear(GL_COLOR_BUFFER_BIT);

#ifdef MEASUREMENT_ENABLED
    ATrace_beginSection("ZeroStall_UpdateGeometry");
#endif
    m_Geometry->Update(m_BakedUVs);
#ifdef MEASUREMENT_ENABLED
    ATrace_endSection();
#endif

    m_Shader->Use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, m_CameraTextureId);
    glUniform1i(glGetUniformLocation(m_Shader->GetId(), "sCamera"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_FontAtlasTextureId);
    glUniform1i(glGetUniformLocation(m_Shader->GetId(), "sFont"), 1);

    glBindBuffer(GL_ARRAY_BUFFER, m_Geometry->GetVboId());
    GLsizei stride = 7 * sizeof(float);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glDisable(GL_BLEND);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_TRIANGLES, 6, 48);

#ifdef MEASUREMENT_ENABLED
    ATrace_beginSection("ZeroStall_eglSwapBuffers");
#endif
    m_Egl->SwapBuffers();

    // Unlock the memory so the camera hardware can write a new frame into it
    if (m_CurrentBuffer) {
        AHardwareBuffer_release(m_CurrentBuffer);
        m_CurrentBuffer = nullptr;
    }

#ifdef MEASUREMENT_ENABLED
    ATrace_endSection();

    ATrace_endSection(); // End DrawFrame

    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> frameTime = endTime - startTime;

    m_FrameCount++;
    if (m_FrameCount % 60 == 0) { // Log once per second at 60fps
        LOGI("Frame Time: %.2f ms | FPS: %.1f", frameTime.count(), 1000.0 / frameTime.count());
    }
#endif
}
