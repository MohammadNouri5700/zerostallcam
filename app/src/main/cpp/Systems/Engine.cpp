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
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    mTimestamp.epochSeconds = (float)seconds.count();
}

void Engine::DrawFrame() {
#ifdef MEASUREMENT_ENABLED
    auto frameStart = std::chrono::steady_clock::now();
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
    std::chrono::duration<double, std::milli> cpuDuration = frameEnd - frameStart;
    mGraphics.totalCpuTimeMs += cpuDuration.count();
    mGraphics.measurementFrameCount++;

    // Collect GPU time if available (don't stall)
    GLuint gpuTimeAvailable = 0;
    glGetQueryObjectuiv(mGraphics.gpuTimerQuery, GL_QUERY_RESULT_AVAILABLE, &gpuTimeAvailable);
    if (gpuTimeAvailable && mGraphics.glGetQueryObjectui64vEXT) {
        GLuint64 gpuTimeNs = 0;
        mGraphics.glGetQueryObjectui64vEXT(mGraphics.gpuTimerQuery, GL_QUERY_RESULT, &gpuTimeNs);
        mGraphics.totalGpuTimeMs += (double)gpuTimeNs / 1000000.0;
    }

    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsedSinceLog = currentTime - mGraphics.lastLogTime;

    if (elapsedSinceLog.count() >= 1000.0) {
        double avgFps = (double)mGraphics.measurementFrameCount / (elapsedSinceLog.count() / 1000.0);
        double avgCpu = mGraphics.totalCpuTimeMs / (double)mGraphics.measurementFrameCount;
        double avgGpu = mGraphics.totalGpuTimeMs / (double)mGraphics.measurementFrameCount;

        LOGI("Performance Summary | FPS: %.1f | CPU: %.2f ms | GPU: %.2f ms | Cache: %zu",
             avgFps, avgCpu, avgGpu, mGraphics.eglImageCache.size());

        // Reset metrics
        mGraphics.lastLogTime = currentTime;
        mGraphics.measurementFrameCount = 0;
        mGraphics.totalCpuTimeMs = 0;
        mGraphics.totalGpuTimeMs = 0;
    }
#endif
}

} // namespace ecs
