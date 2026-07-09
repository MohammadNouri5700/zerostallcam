#include "EglManager.h"
#include <android/log.h>
#include <android/trace.h>

#define LOG_TAG "ZeroStallEglManager"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

namespace ecs {

bool EglManager::Init(GraphicsComponent& graphics, ANativeWindow* window) {
    graphics.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(graphics.display, nullptr, nullptr);

    const EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_NONE
    };

    EGLint numConfigs;
    eglChooseConfig(graphics.display, attribs, &graphics.config, 1, &numConfigs);

    graphics.surface = eglCreateWindowSurface(graphics.display, graphics.config, window, nullptr);

    const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };
    graphics.context = eglCreateContext(graphics.display, graphics.config, EGL_NO_CONTEXT, contextAttribs);

    if (eglMakeCurrent(graphics.display, graphics.surface, graphics.surface, graphics.context) == EGL_FALSE) {
        LOGE("eglMakeCurrent failed");
        return false;
    }

    LOGI("GLES Version: %s", glGetString(GL_VERSION));

    graphics.eglCreateImageKHR = (PFNEGLCREATEIMAGEKHRPROC)eglGetProcAddress("eglCreateImageKHR");
    graphics.eglDestroyImageKHR = (PFNEGLDESTROYIMAGEKHRPROC)eglGetProcAddress("eglDestroyImageKHR");
    graphics.glEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)eglGetProcAddress("glEGLImageTargetTexture2DOES");
    graphics.eglGetNativeClientBufferANDROID = (PFNEGLGETNATIVECLIENTBUFFERANDROIDPROC)eglGetProcAddress("eglGetNativeClientBufferANDROID");

#ifdef MEASUREMENT_ENABLED
    graphics.glGetQueryObjectui64vEXT = (void (*)(GLuint, GLenum, GLuint64*))eglGetProcAddress("glGetQueryObjectui64vEXT");
#endif

    return true;
}

void EglManager::Terminate(GraphicsComponent& graphics) {
    if (graphics.display != EGL_NO_DISPLAY) {
        eglMakeCurrent(graphics.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (graphics.context != EGL_NO_CONTEXT) eglDestroyContext(graphics.display, graphics.context);
        if (graphics.surface != EGL_NO_SURFACE) eglDestroySurface(graphics.display, graphics.surface);
        eglTerminate(graphics.display);
    }
    graphics.display = EGL_NO_DISPLAY;
    graphics.surface = EGL_NO_SURFACE;
    graphics.context = EGL_NO_CONTEXT;
}

void EglManager::SwapBuffers(GraphicsComponent& graphics) {
#ifdef MEASUREMENT_ENABLED
    ATrace_beginSection("ZeroStall_eglSwapBuffers");
#endif
    eglSwapBuffers(graphics.display, graphics.surface);
#ifdef MEASUREMENT_ENABLED
    ATrace_endSection();
#endif
}

} // namespace ecs
