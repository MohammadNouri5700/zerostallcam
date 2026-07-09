#ifndef ZEROSTALLCAM_COMPONENTS_TRANSFORMCOMPONENT_H
#define ZEROSTALLCAM_COMPONENTS_TRANSFORMCOMPONENT_H

namespace ecs {

struct alignas(16) TransformComponent {
    float sw, sh, cw, ch;
    float rot, p1, p2, p3;
};

} // namespace ecs

#endif //ZEROSTALLCAM_COMPONENTS_TRANSFORMCOMPONENT_H
