#ifndef ZEROSTALLCAM_RENDERER_ENGINE_H
#define ZEROSTALLCAM_RENDERER_ENGINE_H

#include "../graphics/EglManager.h"
#include "../graphics/ShaderProgram.h"
#include "../graphics/SceneGeometry.h"
#include <EGL/eglext.h>
#include <GLES2/gl2ext.h>
#include <memory>

class Renderer {
public:
    Renderer();
    ~Renderer();

    void Init(ANativeWindow* window);
    void UpdateCameraBuffer(AHardwareBuffer* buffer);
    void CreateHDFontAtlas(int width, int height, void* pixels);
    void DrawFrame();

private:
    void CalculatePreBakedUVs(int screenW, int screenH, int camW, int camH, int rotationDeg, float* outUVs);

    std::unique_ptr<EglManager> m_Egl;
    std::unique_ptr<ShaderProgram> m_Shader;
    std::unique_ptr<SceneGeometry> m_Geometry;

    GLuint m_CameraTextureId;
    GLuint m_FontAtlasTextureId;
    EGLImageKHR m_EglImage;
    AHardwareBuffer* m_CurrentBuffer;

    float m_BakedUVs[8];
    int m_WindowWidth, m_WindowHeight;
    static constexpr int kCameraWidth = 1920;
    static constexpr int kCameraHeight = 1080;

    PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR;
    PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR;
    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES;
    PFNEGLGETNATIVECLIENTBUFFERANDROIDPROC eglGetNativeClientBufferANDROID;

    uint64_t m_FrameCount;
};

#endif //ZEROSTALLCAM_RENDERER_ENGINE_H
