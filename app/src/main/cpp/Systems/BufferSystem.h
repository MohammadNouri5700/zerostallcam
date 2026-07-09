#ifndef ZEROSTALLCAM_SYSTEMS_BUFFERSYSTEM_H
#define ZEROSTALLCAM_SYSTEMS_BUFFERSYSTEM_H

#include "../Components/BufferComponent.h"

namespace ecs {

class BufferSystem {
public:
    static void Init(BufferComponent& buffer, int width, int height);
    static void Deinit(BufferComponent& buffer);
    static AHardwareBuffer* AcquireLatestBuffer(BufferComponent& buffer);

private:
    static void OnImageAvailable(void* context, AImageReader* reader);
};

} // namespace ecs

#endif //ZEROSTALLCAM_SYSTEMS_BUFFERSYSTEM_H
