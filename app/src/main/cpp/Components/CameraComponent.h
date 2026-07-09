#ifndef ZEROSTALLCAM_COMPONENTS_CAMERACOMPONENT_H
#define ZEROSTALLCAM_COMPONENTS_CAMERACOMPONENT_H

#include <camera/NdkCameraManager.h>
#include <camera/NdkCameraDevice.h>
#include <camera/NdkCameraCaptureSession.h>

namespace ecs {

struct CameraComponent {
    ACameraManager* manager = nullptr;
    ACameraDevice* device = nullptr;
    ACameraCaptureSession* session = nullptr;
    ACaptureRequest* request = nullptr;
    ACameraOutputTarget* outputTarget = nullptr;

    ACameraDevice_StateCallbacks deviceCallbacks;
    ACameraCaptureSession_stateCallbacks sessionCallbacks;
};

} // namespace ecs

#endif //ZEROSTALLCAM_COMPONENTS_CAMERACOMPONENT_H
