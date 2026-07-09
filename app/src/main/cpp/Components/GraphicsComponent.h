#ifndef ZEROSTALLCAM_COMPONENTS_GRAPHICSCOMPONENT_H
#define ZEROSTALLCAM_COMPONENTS_GRAPHICSCOMPONENT_H

#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <unordered_map>

namespace ecs {

struct GraphicsComponent {
    // EGL Handles
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;
    EGLConfig config = nullptr;

    // Shader Programs
    GLuint renderProgram = 0;
    GLuint computeProgram = 0;

    // Textures
    GLuint cameraTextureId = 0;
    GLuint fontAtlasTextureId = 0;

    // SSBO for generated vertices
    GLuint vertexSsboId = 0;

    // EGL Image for Hardware Buffer mapping
    EGLImageKHR eglImage = EGL_NO_IMAGE_KHR;
    std::unordered_map<AHardwareBuffer*, EGLImageKHR> eglImageCache;

    // Extension pointers
    PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR = nullptr;
    PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR = nullptr;
    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES = nullptr;
    PFNEGLGETNATIVECLIENTBUFFERANDROIDPROC eglGetNativeClientBufferANDROID = nullptr;

    uint64_t frameCount = 0;

#ifdef MEASUREMENT_ENABLED
    void (*glGetQueryObjectui64vEXT)(GLuint, GLenum, GLuint64*) = nullptr;
    GLuint gpuTimerQuery = 0;
    bool gpuTimerSupported = false;
    double totalCpuTimeMs = 0;
    double totalGpuTimeMs = 0;
    std::chrono::steady_clock::time_point lastLogTime;
    uint32_t measurementFrameCount = 0;
#endif
};

} // namespace ecs

#endif //ZEROSTALLCAM_COMPONENTS_GRAPHICSCOMPONENT_H
