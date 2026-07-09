#include "ImageProvider.h"

#ifndef AHARDWAREBUFFER_USAGE_CAMERA_WRITE
#define AHARDWAREBUFFER_USAGE_CAMERA_WRITE 0x20000
#endif

ImageProvider::ImageProvider() : m_ImageReader(nullptr), m_Window(nullptr) {
    m_Listener.context = this;
    m_Listener.onImageAvailable = OnImageAvailable;
}

ImageProvider::~ImageProvider() {
    Terminate();
}

void ImageProvider::Init(int width, int height) {
    AImageReader_newWithUsage(width, height, AIMAGE_FORMAT_YUV_420_888,
        AHARDWAREBUFFER_USAGE_GPU_SAMPLED_IMAGE | AHARDWAREBUFFER_USAGE_CAMERA_WRITE,
        3, &m_ImageReader);
    AImageReader_setImageListener(m_ImageReader, &m_Listener);
    AImageReader_getWindow(m_ImageReader, &m_Window);
}

void ImageProvider::Terminate() {
    if (m_ImageReader) {
        AImageReader_delete(m_ImageReader);
        m_ImageReader = nullptr;
        m_Window = nullptr;
    }
}

AHardwareBuffer* ImageProvider::AcquireLatestBuffer() {
    AImage* image = nullptr;
    if (AImageReader_acquireLatestImage(m_ImageReader, &image) != AMEDIA_OK) return nullptr;
    AHardwareBuffer* buffer = nullptr;
    AImage_getHardwareBuffer(image, &buffer);

    if (buffer) {
        // Tell the OS: "Do not recycle this memory! I am using it!"
        AHardwareBuffer_acquire(buffer);
    }

    // Now it is safe to delete the wrapper
    AImage_delete(image);
    return buffer;
}

void ImageProvider::OnImageAvailable(void* context, AImageReader* reader) {}
