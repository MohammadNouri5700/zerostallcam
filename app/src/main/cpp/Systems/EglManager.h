#ifndef ZEROSTALLCAM_SYSTEMS_EGL_MANAGER_H
#define ZEROSTALLCAM_SYSTEMS_EGL_MANAGER_H

#include "../Components/GraphicsComponent.h"
#include <android/native_window.h>

namespace ecs {

class EglManager {
public:
    static bool Init(GraphicsComponent& graphics, ANativeWindow* window);
    static void Terminate(GraphicsComponent& graphics);
    static void SwapBuffers(GraphicsComponent& graphics);
};

} // namespace ecs

#endif //ZEROSTALLCAM_SYSTEMS_EGL_MANAGER_H
