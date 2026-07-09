#include "BufferSystem.h"

#ifndef AHARDWAREBUFFER_USAGE_CAMERA_WRITE
#define AHARDWAREBUFFER_USAGE_CAMERA_WRITE 0x20000
#endif

namespace ecs {

void BufferSystem::Init(BufferComponent& buffer, int width, int height) {
    buffer.listener.context = &buffer;
    buffer.listener.onImageAvailable = OnImageAvailable;

    AImageReader_newWithUsage(width, height, AIMAGE_FORMAT_YUV_420_888,
        AHARDWAREBUFFER_USAGE_GPU_SAMPLED_IMAGE | AHARDWAREBUFFER_USAGE_CAMERA_WRITE,
        3, &buffer.reader);
    AImageReader_setImageListener(buffer.reader, &buffer.listener);
    AImageReader_getWindow(buffer.reader, &buffer.window);
}

void BufferSystem::Deinit(BufferComponent& buffer) {
    if (buffer.reader) {
        AImageReader_delete(buffer.reader);
        buffer.reader = nullptr;
        buffer.window = nullptr;
    }
}

AHardwareBuffer* BufferSystem::AcquireLatestBuffer(BufferComponent& buffer) {
    AImage* img = nullptr;
    if (AImageReader_acquireLatestImage(buffer.reader, &img) != AMEDIA_OK) return nullptr;
    AHardwareBuffer* hardwareBuffer = nullptr;
    AImage_getHardwareBuffer(img, &hardwareBuffer);
    if (hardwareBuffer) AHardwareBuffer_acquire(hardwareBuffer);
    AImage_delete(img);
    return hardwareBuffer;
}

void BufferSystem::OnImageAvailable(void* context, AImageReader* reader) {}

} // namespace ecs
