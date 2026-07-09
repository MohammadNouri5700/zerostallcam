#ifndef ZEROSTALLCAM_SYSTEMS_ENGINE_H
#define ZEROSTALLCAM_SYSTEMS_ENGINE_H

#include "../Components/CameraComponent.h"
#include "../Components/BufferComponent.h"
#include "../Components/GraphicsComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/TimestampComponent.h"
#include <android/native_window.h>
#include <android/asset_manager.h>

namespace ecs {

class Engine {
public:
    Engine();
    ~Engine();

    void Init(ANativeWindow* window, AAssetManager* assetManager);
    void StartCamera();
    void SetFontAtlas(int width, int height, void* pixels);
    void DrawFrame();

private:
    CameraComponent mCamera;
    BufferComponent mBuffer;
    GraphicsComponent mGraphics;
    TransformComponent mTransform;
    TimestampComponent mTimestamp;
    AAssetManager* mAssetManager = nullptr;

    void UpdateTimestamp();
};

} // namespace ecs

#endif //ZEROSTALLCAM_SYSTEMS_ENGINE_H
