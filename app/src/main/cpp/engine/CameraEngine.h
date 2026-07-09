#ifndef ZEROSTALLCAM_CAMERA_ENGINE_H
#define ZEROSTALLCAM_CAMERA_ENGINE_H

#include "../camera/NDKCamera.h"
#include "../camera/ImageProvider.h"
#include <memory>

class CameraEngine {
public:
    CameraEngine();
    ~CameraEngine();

    void StartCapture();
    void StopCapture();
    AHardwareBuffer* GetLatestBuffer();

private:
    std::unique_ptr<NDKCamera> m_Camera;
    std::unique_ptr<ImageProvider> m_Provider;
};

#endif //ZEROSTALLCAM_CAMERA_ENGINE_H
