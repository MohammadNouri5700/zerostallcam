#ifndef ZEROSTALLCAM_COMPONENTS_BUFFERCOMPONENT_H
#define ZEROSTALLCAM_COMPONENTS_BUFFERCOMPONENT_H

#include <media/NdkImageReader.h>
#include <android/hardware_buffer.h>

namespace ecs {

struct BufferComponent {
    AImageReader* reader = nullptr;
    ANativeWindow* window = nullptr;
    AImageReader_ImageListener listener;
};

} // namespace ecs

#endif //ZEROSTALLCAM_COMPONENTS_BUFFERCOMPONENT_H
