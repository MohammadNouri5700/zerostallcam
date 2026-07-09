#ifndef ZEROSTALLCAM_SCENEGEOMETRY_H
#define ZEROSTALLCAM_SCENEGEOMETRY_H

#include <GLES3/gl3.h>
#include <vector>

class SceneGeometry {
public:
    SceneGeometry();
    ~SceneGeometry();

    void Create();
    void Update(const float* bakedUVs);
    void Terminate();
    GLuint GetVboId() const { return m_VboId; }

    // Text Layout Constants (NDC: -1 to 1)
    static constexpr float kTextPosX = -0.99f;
    static constexpr float kTextPosY = 0.99f;
    static constexpr float kTextCharWidth = 0.06f;
    static constexpr float kTextCharHeight = 0.05f;

private:
    GLuint m_VboId;
};

#endif //ZEROSTALLCAM_SCENEGEOMETRY_H
