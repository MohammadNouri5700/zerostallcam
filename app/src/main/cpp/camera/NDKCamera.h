#ifndef ZEROSTALLCAM_NDKCAMERA_H
#define ZEROSTALLCAM_NDKCAMERA_H

#include <camera/NdkCameraManager.h>
#include <camera/NdkCameraDevice.h>
#include <camera/NdkCameraCaptureSession.h>
#include <android/native_window.h>

class NDKCamera {
public:
    NDKCamera();
    ~NDKCamera();

    void Open();
    void Close();
    void StartCapture(ANativeWindow* window);
    void StopCapture();

private:
    ACameraManager* m_CameraManager;
    ACameraDevice* m_CameraDevice;
    ACameraCaptureSession* m_CaptureSession;
    ACaptureRequest* m_CaptureRequest;
    ACameraOutputTarget* m_OutputTarget;

    static void OnDeviceDisconnected(void* context, ACameraDevice* device);
    static void OnDeviceError(void* context, ACameraDevice* device, int error);
    ACameraDevice_StateCallbacks m_DeviceCallbacks;

    static void OnSessionActive(void* context, ACameraCaptureSession* session);
    static void OnSessionReady(void* context, ACameraCaptureSession* session);
    static void OnSessionClosed(void* context, ACameraCaptureSession* session);
    ACameraCaptureSession_stateCallbacks m_SessionCallbacks;
};

#endif //ZEROSTALLCAM_NDKCAMERA_H
