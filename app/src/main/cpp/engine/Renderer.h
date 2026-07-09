#ifndef ZEROSTALLCAM_RENDERER_ENGINE_H
#define ZEROSTALLCAM_RENDERER_ENGINE_H

#include "../graphics/EglManager.h"
#include "../graphics/ShaderProgram.h"
#include "../graphics/FontAtlas.h"
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
    void DrawFrame();

private:
    std::unique_ptr<EglManager> m_Egl;
    std::unique_ptr<ShaderProgram> m_Shader;
    std::unique_ptr<FontAtlas> m_FontAtlas;
    std::unique_ptr<SceneGeometry> m_Geometry;

    GLuint m_CameraTextureId;
    EGLImageKHR m_EglImage;

    PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR;
    PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR;
    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES;
    PFNEGLGETNATIVECLIENTBUFFERANDROIDPROC eglGetNativeClientBufferANDROID;

    uint64_t m_FrameCount;
};

#endif //ZEROSTALLCAM_RENDERER_ENGINE_H
