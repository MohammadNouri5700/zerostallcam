#ifndef ZEROSTALLCAM_COMPONENTS_HARDWAREBUFFERLAYOUTS_H
#define ZEROSTALLCAM_COMPONENTS_HARDWAREBUFFERLAYOUTS_H

#include <GLES3/gl31.h>

namespace ecs {

// Vertex structure exactly as it will be seen by the GPU in the SSBO
// Aligned to 16 bytes (std140/std430 compatibility)
struct alignas(16) Vertex {
    float pos[4];    // vec4 position
    float tex[2];    // vec2 texCoord
    float type;      // float type (0=camera, 1=font)
    float padding;   // Explicit padding to maintain 16-byte alignment
};

} // namespace ecs

#endif //ZEROSTALLCAM_COMPONENTS_HARDWAREBUFFERLAYOUTS_H
