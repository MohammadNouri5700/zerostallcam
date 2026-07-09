#include "EglManager.h"

EglManager::EglManager() :
    m_EglDisplay(EGL_NO_DISPLAY), m_EglSurface(EGL_NO_SURFACE),
    m_EglContext(EGL_NO_CONTEXT), m_EglConfig(nullptr) {}

EglManager::~EglManager() {
    Terminate();
}

bool EglManager::Init(ANativeWindow* window) {
    m_EglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(m_EglDisplay, nullptr, nullptr);

    const EGLint configAttribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8,
        EGL_NONE
    };
    EGLint numConfigs;
    eglChooseConfig(m_EglDisplay, configAttribs, &m_EglConfig, 1, &numConfigs);

    const EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
    m_EglContext = eglCreateContext(m_EglDisplay, m_EglConfig, EGL_NO_CONTEXT, contextAttribs);

    m_EglSurface = eglCreateWindowSurface(m_EglDisplay, m_EglConfig, window, nullptr);
    return eglMakeCurrent(m_EglDisplay, m_EglSurface, m_EglSurface, m_EglContext);
}

void EglManager::Terminate() {
    if (m_EglDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(m_EglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (m_EglContext != EGL_NO_CONTEXT) eglDestroyContext(m_EglDisplay, m_EglContext);
        if (m_EglSurface != EGL_NO_SURFACE) eglDestroySurface(m_EglDisplay, m_EglSurface);
        eglTerminate(m_EglDisplay);
    }
    m_EglDisplay = EGL_NO_DISPLAY;
    m_EglContext = EGL_NO_CONTEXT;
    m_EglSurface = EGL_NO_SURFACE;
}

void EglManager::SwapBuffers() {
    if (m_EglDisplay != EGL_NO_DISPLAY && m_EglSurface != EGL_NO_SURFACE) {
        eglSwapBuffers(m_EglDisplay, m_EglSurface);
    }
}
