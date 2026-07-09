#ifndef ZEROSTALLCAM_COMPONENTS_TIMESTAMPCOMPONENT_H
#define ZEROSTALLCAM_COMPONENTS_TIMESTAMPCOMPONENT_H

#include <cstdint>

namespace ecs {

struct alignas(16) TimestampComponent {
    int32_t epochSeconds;
    int32_t p1, p2, p3; // Padding to 16 bytes
};

} // namespace ecs

#endif //ZEROSTALLCAM_COMPONENTS_TIMESTAMPCOMPONENT_H
