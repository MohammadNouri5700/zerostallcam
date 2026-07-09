#ifndef ZEROSTALLCAM_SCENEGEOMETRY_H
#define ZEROSTALLCAM_SCENEGEOMETRY_H

#include <GLES3/gl3.h>
#include <vector>

class SceneGeometry {
public:
    SceneGeometry();
    ~SceneGeometry();

    void Create();
    void Update();
    void Terminate();
    GLuint GetVboId() const { return m_VboId; }

private:
    GLuint m_VboId;
};

#endif //ZEROSTALLCAM_SCENEGEOMETRY_H
