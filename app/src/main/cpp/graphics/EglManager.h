#ifndef ZEROSTALLCAM_EGLMANAGER_H
#define ZEROSTALLCAM_EGLMANAGER_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <android/native_window.h>

class EglManager {
public:
    EglManager();
    ~EglManager();

    bool Init(ANativeWindow* window);
    void Terminate();
    void SwapBuffers();

    EGLDisplay GetDisplay() const { return m_EglDisplay; }
    EGLSurface GetSurface() const { return m_EglSurface; }
    EGLContext GetContext() const { return m_EglContext; }

private:
    EGLDisplay m_EglDisplay;
    EGLSurface m_EglSurface;
    EGLContext m_EglContext;
    EGLConfig m_EglConfig;
};

#endif //ZEROSTALLCAM_EGLMANAGER_H
