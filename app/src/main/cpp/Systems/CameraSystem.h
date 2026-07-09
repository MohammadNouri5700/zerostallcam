#ifndef ZEROSTALLCAM_SYSTEMS_CAMERASYSTEM_H
#define ZEROSTALLCAM_SYSTEMS_CAMERASYSTEM_H

#include "../Components/CameraComponent.h"
#include <android/native_window.h>

namespace ecs {

class CameraSystem {
public:
    static void Init(CameraComponent& camera);
    static void Deinit(CameraComponent& camera);
    static void Open(CameraComponent& camera);
    static void Close(CameraComponent& camera);
    static void StartCapture(CameraComponent& camera, ANativeWindow* window);
    static void StopCapture(CameraComponent& camera);

private:
    static void OnDeviceDisconnected(void* context, ACameraDevice* device);
    static void OnDeviceError(void* context, ACameraDevice* device, int error);
    static void OnSessionActive(void* context, ACameraCaptureSession* session);
    static void OnSessionReady(void* context, ACameraCaptureSession* session);
    static void OnSessionClosed(void* context, ACameraCaptureSession* session);
};

} // namespace ecs

#endif //ZEROSTALLCAM_SYSTEMS_CAMERASYSTEM_H
