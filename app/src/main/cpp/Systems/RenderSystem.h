#ifndef ZEROSTALLCAM_SYSTEMS_RENDERSYSTEM_H
#define ZEROSTALLCAM_SYSTEMS_RENDERSYSTEM_H

#include "../Components/GraphicsComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/TimestampComponent.h"
#include <android/hardware_buffer.h>

namespace ecs {

class RenderSystem {
public:
    static void Init(GraphicsComponent& graphics);
    static void UpdateCameraBuffer(GraphicsComponent& graphics, AHardwareBuffer* buffer);
    static void SetFontAtlas(GraphicsComponent& graphics, int width, int height, void* pixels);
    static void DrawFrame(GraphicsComponent& graphics, const TransformComponent& transform, const TimestampComponent& timestamp);
    static void Deinit(GraphicsComponent& graphics);

private:
    static GLuint mTransformUbo;
    static GLuint mTimestampUbo;
    static GLuint mDummyVao;
};

} // namespace ecs

#endif //ZEROSTALLCAM_SYSTEMS_RENDERSYSTEM_H
