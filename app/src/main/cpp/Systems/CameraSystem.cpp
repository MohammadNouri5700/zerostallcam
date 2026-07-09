#include "CameraSystem.h"
#include <android/log.h>

#define LOG_TAG "ZeroStallCameraSystem"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace ecs {

void CameraSystem::Init(CameraComponent& camera) {
    camera.manager = ACameraManager_create();
    camera.deviceCallbacks.context = &camera;
    camera.deviceCallbacks.onDisconnected = OnDeviceDisconnected;
    camera.deviceCallbacks.onError = OnDeviceError;
    camera.sessionCallbacks.context = &camera;
    camera.sessionCallbacks.onActive = OnSessionActive;
    camera.sessionCallbacks.onReady = OnSessionReady;
    camera.sessionCallbacks.onClosed = OnSessionClosed;
}

void CameraSystem::Deinit(CameraComponent& camera) {
    StopCapture(camera);
    Close(camera);
    if (camera.manager) ACameraManager_delete(camera.manager);
    camera.manager = nullptr;
}

void CameraSystem::Open(CameraComponent& camera) {
    ACameraIdList* idList = nullptr;
    ACameraManager_getCameraIdList(camera.manager, &idList);
    const char* selectedCameraId = nullptr;
    for (int i = 0; i < idList->numCameras; ++i) {
        const char* id = idList->cameraIds[i];
        ACameraMetadata* metadata = nullptr;
        ACameraManager_getCameraCharacteristics(camera.manager, id, &metadata);
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
        ACameraManager_openCamera(camera.manager, selectedCameraId, &camera.deviceCallbacks, &camera.device);
    }
    ACameraManager_deleteCameraIdList(idList);
}

void CameraSystem::Close(CameraComponent& camera) {
    if (camera.device) {
        ACameraDevice_close(camera.device);
        camera.device = nullptr;
    }
}

void CameraSystem::StartCapture(CameraComponent& camera, ANativeWindow* window) {
    if (!camera.device || !window) return;
    ACameraDevice_createCaptureRequest(camera.device, TEMPLATE_PREVIEW, &camera.request);
    ACameraOutputTarget_create(window, &camera.outputTarget);
    ACaptureRequest_addTarget(camera.request, camera.outputTarget);
    ACaptureSessionOutputContainer* container = nullptr;
    ACaptureSessionOutputContainer_create(&container);
    ACaptureSessionOutput* output = nullptr;
    ACaptureSessionOutput_create(window, &output);
    ACaptureSessionOutputContainer_add(container, output);
    ACameraDevice_createCaptureSession(camera.device, container, &camera.sessionCallbacks, &camera.session);
    ACameraCaptureSession_setRepeatingRequest(camera.session, nullptr, 1, &camera.request, nullptr);
    ACaptureSessionOutputContainer_free(container);
}

void CameraSystem::StopCapture(CameraComponent& camera) {
    if (camera.session) {
        ACameraCaptureSession_stopRepeating(camera.session);
        ACameraCaptureSession_close(camera.session);
        camera.session = nullptr;
    }
    if (camera.request) {
        ACaptureRequest_free(camera.request);
        camera.request = nullptr;
    }
    if (camera.outputTarget) {
        ACameraOutputTarget_free(camera.outputTarget);
        camera.outputTarget = nullptr;
    }
}

void CameraSystem::OnDeviceDisconnected(void* context, ACameraDevice* device) { LOGI("Camera disconnected"); }
void CameraSystem::OnDeviceError(void* context, ACameraDevice* device, int error) { LOGE("Camera error: %d", error); }
void CameraSystem::OnSessionActive(void* context, ACameraCaptureSession* session) { LOGI("Session active"); }
void CameraSystem::OnSessionReady(void* context, ACameraCaptureSession* session) { LOGI("Session ready"); }
void CameraSystem::OnSessionClosed(void* context, ACameraCaptureSession* session) { LOGI("Session closed"); }

} // namespace ecs
