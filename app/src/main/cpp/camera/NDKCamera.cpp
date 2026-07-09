#include "NDKCamera.h"
#include <android/log.h>

#define LOG_TAG "ZeroStallNDKCamera"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

NDKCamera::NDKCamera() :
    m_CameraManager(nullptr), m_CameraDevice(nullptr),
    m_CaptureSession(nullptr), m_CaptureRequest(nullptr),
    m_OutputTarget(nullptr) {

    m_CameraManager = ACameraManager_create();
    m_DeviceCallbacks.context = this;
    m_DeviceCallbacks.onDisconnected = OnDeviceDisconnected;
    m_DeviceCallbacks.onError = OnDeviceError;
    m_SessionCallbacks.context = this;
    m_SessionCallbacks.onActive = OnSessionActive;
    m_SessionCallbacks.onReady = OnSessionReady;
    m_SessionCallbacks.onClosed = OnSessionClosed;
}

NDKCamera::~NDKCamera() {
    StopCapture();
    Close();
    if (m_CameraManager) ACameraManager_delete(m_CameraManager);
}

void NDKCamera::Open() {
    ACameraIdList* idList = nullptr;
    ACameraManager_getCameraIdList(m_CameraManager, &idList);
    const char* selectedCameraId = nullptr;
    for (int i = 0; i < idList->numCameras; ++i) {
        const char* id = idList->cameraIds[i];
        ACameraMetadata* metadata = nullptr;
        ACameraManager_getCameraCharacteristics(m_CameraManager, id, &metadata);
        ACameraMetadata_const_entry entry;
        ACameraMetadata_getConstEntry(metadata, ACAMERA_LENS_FACING, &entry);
        if (entry.data.u8[0] == ACAMERA_LENS_FACING_BACK) {
            selectedCameraId = id;
            ACameraMetadata_free(metadata);
            break;
        }
        ACameraMetadata_free(metadata);
    }
    if (selectedCameraId) {
        ACameraManager_openCamera(m_CameraManager, selectedCameraId, &m_DeviceCallbacks, &m_CameraDevice);
    }
    ACameraManager_deleteCameraIdList(idList);
}

void NDKCamera::Close() {
    if (m_CameraDevice) {
        ACameraDevice_close(m_CameraDevice);
        m_CameraDevice = nullptr;
    }
}

void NDKCamera::StartCapture(ANativeWindow* window) {
    if (!m_CameraDevice || !window) return;
    ACameraDevice_createCaptureRequest(m_CameraDevice, TEMPLATE_PREVIEW, &m_CaptureRequest);
    ACameraOutputTarget_create(window, &m_OutputTarget);
    ACaptureRequest_addTarget(m_CaptureRequest, m_OutputTarget);
    ACaptureSessionOutputContainer* container = nullptr;
    ACaptureSessionOutputContainer_create(&container);
    ACaptureSessionOutput* output = nullptr;
    ACaptureSessionOutput_create(window, &output);
    ACaptureSessionOutputContainer_add(container, output);
    ACameraDevice_createCaptureSession(m_CameraDevice, container, &m_SessionCallbacks, &m_CaptureSession);
    ACameraCaptureSession_setRepeatingRequest(m_CaptureSession, nullptr, 1, &m_CaptureRequest, nullptr);
    ACaptureSessionOutputContainer_free(container);
}

void NDKCamera::StopCapture() {
    if (m_CaptureSession) {
        ACameraCaptureSession_stopRepeating(m_CaptureSession);
        ACameraCaptureSession_close(m_CaptureSession);
        m_CaptureSession = nullptr;
    }
    if (m_CaptureRequest) {
        ACaptureRequest_free(m_CaptureRequest);
        m_CaptureRequest = nullptr;
    }
    if (m_OutputTarget) {
        ACameraOutputTarget_free(m_OutputTarget);
        m_OutputTarget = nullptr;
    }
}

void NDKCamera::OnDeviceDisconnected(void* context, ACameraDevice* device) { LOGI("Camera disconnected"); }
void NDKCamera::OnDeviceError(void* context, ACameraDevice* device, int error) { LOGE("Camera error: %d", error); }
void NDKCamera::OnSessionActive(void* context, ACameraCaptureSession* session) { LOGI("Session active"); }
void NDKCamera::OnSessionReady(void* context, ACameraCaptureSession* session) { LOGI("Session ready"); }
void NDKCamera::OnSessionClosed(void* context, ACameraCaptureSession* session) { LOGI("Session closed"); }
