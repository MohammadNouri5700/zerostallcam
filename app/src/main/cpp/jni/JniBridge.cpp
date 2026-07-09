#include <jni.h>
#include <android/native_window_jni.h>
#include <android/bitmap.h>
#include "../Systems/Engine.h"
#include <memory>

static std::unique_ptr<ecs::Engine> g_Engine;

extern "C" JNIEXPORT void JNICALL
Java_com_example_zerostallcam_ZeroStallCamView_nativeSurfaceCreated(JNIEnv* env, jobject thiz, jobject surface) {
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    g_Engine = std::make_unique<ecs::Engine>();
    g_Engine->Init(window);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_zerostallcam_ZeroStallCamView_nativeStartCamera(JNIEnv* env, jobject thiz) {
    if (g_Engine) {
        g_Engine->StartCamera();
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_zerostallcam_ZeroStallCamView_nativeSurfaceDestroyed(JNIEnv* env, jobject thiz) {
    g_Engine.reset();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_zerostallcam_ZeroStallCamView_nativeSetFontAtlas(JNIEnv* env, jobject thiz, jobject bitmap) {
    AndroidBitmapInfo info;
    void* pixels;
    if (AndroidBitmap_getInfo(env, bitmap, &info) < 0) return;
    if (AndroidBitmap_lockPixels(env, bitmap, &pixels) < 0) return;
    if (g_Engine) {
        g_Engine->SetFontAtlas(info.width, info.height, pixels);
    }
    AndroidBitmap_unlockPixels(env, bitmap);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_zerostallcam_ZeroStallCamView_nativeDrawFrame(JNIEnv* env, jobject thiz) {
    if (g_Engine) {
        g_Engine->DrawFrame();
    }
}
