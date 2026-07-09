#ifndef ZEROSTALLCAM_IMAGEPROVIDER_H
#define ZEROSTALLCAM_IMAGEPROVIDER_H

#include <media/NdkImageReader.h>
#include <android/hardware_buffer.h>
#include <android/native_window.h>

class ImageProvider {
public:
    ImageProvider();
    ~ImageProvider();

    void Init(int width, int height);
    void Terminate();
    ANativeWindow* GetWindow() const { return m_Window; }
    AHardwareBuffer* AcquireLatestBuffer();

private:
    static void OnImageAvailable(void* context, AImageReader* reader);
    AImageReader* m_ImageReader;
    ANativeWindow* m_Window;
    AImageReader_ImageListener m_Listener;
};

#endif //ZEROSTALLCAM_IMAGEPROVIDER_H
