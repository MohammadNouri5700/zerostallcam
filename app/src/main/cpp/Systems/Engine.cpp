#include "Engine.h"
#include "EglManager.h"
#include "CameraSystem.h"
#include "BufferSystem.h"
#include "RenderSystem.h"
#include <ctime>
#include <chrono>
#include <android/log.h>
#include <android/trace.h>

#define LOG_TAG "ZeroStallEngine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

namespace ecs {

Engine::Engine() {
    CameraSystem::Init(mCamera);
    BufferSystem::Init(mBuffer, 1920, 1080);
    CameraSystem::Open(mCamera);

    mTransform.cw = 1920.0f;
    mTransform.ch = 1080.0f;
    mTransform.rot = 90.0f;
}

Engine::~Engine() {
    RenderSystem::Deinit(mGraphics);
    EglManager::Terminate(mGraphics);
    CameraSystem::Deinit(mCamera);
    BufferSystem::Deinit(mBuffer);
}

void Engine::Init(ANativeWindow* window) {
    EglManager::Init(mGraphics, window);
    RenderSystem::Init(mGraphics);

    mTransform.sw = (float)ANativeWindow_getWidth(window);
    mTransform.sh = (float)ANativeWindow_getHeight(window);
}

void Engine::StartCamera() {
    CameraSystem::StartCapture(mCamera, mBuffer.window);
}

void Engine::SetFontAtlas(int width, int height, void* pixels) {
    RenderSystem::SetFontAtlas(mGraphics, width, height, pixels);
}

void Engine::UpdateTimestamp() {
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    mTimestamp.h = (float)timeinfo->tm_hour;
    mTimestamp.m = (float)timeinfo->tm_min;
    mTimestamp.s = (float)timeinfo->tm_sec;
    mTimestamp.p = 0.0f;

    static int lastSec = -1;
    if ((int)mTimestamp.s != lastSec) {
        LOGI("Time: %02d:%02d:%02d", (int)mTimestamp.h, (int)mTimestamp.m, (int)mTimestamp.s);
        lastSec = (int)mTimestamp.s;
    }
}

void Engine::DrawFrame() {
#ifdef MEASUREMENT_ENABLED
    static auto lastLogTime = std::chrono::steady_clock::now();
    static int frameCount = 0;
    static double totalWorkTime = 0;

    auto frameStart = std::chrono::steady_clock::now();
    ATrace_beginSection("ZeroStall_TotalFrame");
#endif

    UpdateTimestamp();

    AHardwareBuffer* buffer = BufferSystem::AcquireLatestBuffer(mBuffer);
    if (buffer) {
        RenderSystem::UpdateCameraBuffer(mGraphics, buffer);
    }

    RenderSystem::DrawFrame(mGraphics, mTransform, mTimestamp);

    EglManager::SwapBuffers(mGraphics);

    if (buffer) {
        AHardwareBuffer_release(buffer);
    }

#ifdef MEASUREMENT_ENABLED
    auto frameEnd = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> workDuration = frameEnd - frameStart;
    totalWorkTime += workDuration.count();
    frameCount++;

    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsedSinceLog = currentTime - lastLogTime;

    if (elapsedSinceLog.count() >= 1000.0) {
        double avgFps = (double)frameCount / (elapsedSinceLog.count() / 1000.0);
        double avgWork = totalWorkTime / (double)frameCount;
        LOGI("Performance Summary | FPS: %.1f | Avg Work: %.2f ms | Budget Used: %.1f%%",
             avgFps, avgWork, (avgWork / 16.6) * 100.0);

        // Reset metrics
        lastLogTime = currentTime;
        frameCount = 0;
        totalWorkTime = 0;
    }
    ATrace_endSection();
#endif
}

} // namespace ecs
