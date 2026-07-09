#ifndef ZEROSTALLCAM_FONTATLAS_H
#define ZEROSTALLCAM_FONTATLAS_H

#include <GLES3/gl3.h>
#include <vector>
#include <cstdint>

class FontAtlas {
public:
    FontAtlas();
    ~FontAtlas();

    void Create();
    void Terminate();
    GLuint GetTextureId() const { return m_TextureId; }

private:
    GLuint m_TextureId;
};

#endif //ZEROSTALLCAM_FONTATLAS_H
