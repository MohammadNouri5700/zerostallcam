#include "CameraEngine.h"

CameraEngine::CameraEngine() {
    m_Camera = std::make_unique<NDKCamera>();
    m_Provider = std::make_unique<ImageProvider>();

    m_Provider->Init(1920, 1080);
    m_Camera->Open();
}

CameraEngine::~CameraEngine() {}

void CameraEngine::StartCapture() {
    m_Camera->StartCapture(m_Provider->GetWindow());
}

void CameraEngine::StopCapture() {
    m_Camera->StopCapture();
}

AHardwareBuffer* CameraEngine::GetLatestBuffer() {
    return m_Provider->AcquireLatestBuffer();
}
