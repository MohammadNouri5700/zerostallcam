#include <jni.h>
#include <android/native_window_jni.h>
#include <android/bitmap.h>
#include "../engine/CameraEngine.h"
#include "../engine/Renderer.h"
#include <memory>

static std::unique_ptr<Renderer> g_Renderer;
static std::unique_ptr<CameraEngine> g_CameraEngine;

extern "C" JNIEXPORT void JNICALL
Java_com_example_zerostallcam_ZeroStallCamView_nativeSurfaceCreated(JNIEnv* env, jobject thiz, jobject surface) {
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    g_Renderer = std::make_unique<Renderer>();
    g_Renderer->Init(window);
    g_CameraEngine = std::make_unique<CameraEngine>();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_zerostallcam_ZeroStallCamView_nativeStartCamera(JNIEnv* env, jobject thiz) {
    if (g_CameraEngine) {
        g_CameraEngine->StartCapture();
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_zerostallcam_ZeroStallCamView_nativeSurfaceDestroyed(JNIEnv* env, jobject thiz) {
    g_CameraEngine.reset();
    g_Renderer.reset();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_zerostallcam_ZeroStallCamView_nativeSetFontAtlas(JNIEnv* env, jobject thiz, jobject bitmap) {
    AndroidBitmapInfo info;
    void* pixels;
    if (AndroidBitmap_getInfo(env, bitmap, &info) < 0) return;
    if (AndroidBitmap_lockPixels(env, bitmap, &pixels) < 0) return;
    if (g_Renderer) {
        g_Renderer->CreateHDFontAtlas(info.width, info.height, pixels);
    }
    AndroidBitmap_unlockPixels(env, bitmap);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_zerostallcam_ZeroStallCamView_nativeDrawFrame(JNIEnv* env, jobject thiz) {
    if (g_Renderer && g_CameraEngine) {
        AHardwareBuffer* buffer = g_CameraEngine->GetLatestBuffer();
        if (buffer) {
            g_Renderer->UpdateCameraBuffer(buffer);
        }
        g_Renderer->DrawFrame();
    }
}
