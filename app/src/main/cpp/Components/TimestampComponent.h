#ifndef ZEROSTALLCAM_COMPONENTS_TIMESTAMPCOMPONENT_H
#define ZEROSTALLCAM_COMPONENTS_TIMESTAMPCOMPONENT_H

namespace ecs {

struct alignas(16) TimestampComponent {
    float h, m, s, p;
};

} // namespace ecs

#endif //ZEROSTALLCAM_COMPONENTS_TIMESTAMPCOMPONENT_H
