#include "FontAtlas.h"

FontAtlas::FontAtlas() : m_TextureId(0) {}

FontAtlas::~FontAtlas() {
    Terminate();
}

void FontAtlas::Create() {
    const int atlasW = 128;
    const int atlasH = 16;
    std::vector<uint8_t> pixels(atlasW * atlasH, 0);
    const uint8_t glyphs[11][7] = {
        {0x3E, 0x41, 0x41, 0x41, 0x41, 0x41, 0x3E}, // 0
        {0x08, 0x0C, 0x08, 0x08, 0x08, 0x08, 0x1C}, // 1
        {0x3E, 0x41, 0x01, 0x02, 0x04, 0x08, 0x7F}, // 2
        {0x3E, 0x41, 0x01, 0x0E, 0x01, 0x41, 0x3E}, // 3
        {0x02, 0x06, 0x0A, 0x12, 0x7F, 0x02, 0x02}, // 4
        {0x7F, 0x40, 0x7E, 0x01, 0x01, 0x41, 0x3E}, // 5
        {0x3E, 0x41, 0x40, 0x7E, 0x41, 0x41, 0x3E}, // 6
        {0x7F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08}, // 7
        {0x3E, 0x41, 0x41, 0x3E, 0x41, 0x41, 0x3E}, // 8
        {0x3E, 0x41, 0x41, 0x3F, 0x01, 0x41, 0x3E}, // 9
        {0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00}  // :
    };
    for (int i = 0; i < 11; ++i) {
        for (int y = 0; y < 7; ++y) {
            for (int x = 0; x < 8; ++x) {
                if (glyphs[i][y] & (1 << (7 - x))) {
                    pixels[(y + 4) * atlasW + (i * 10 + x + 1)] = 255;
                }
            }
        }
    }
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, atlasW, atlasH, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void FontAtlas::Terminate() {
    if (m_TextureId != 0) {
        glDeleteTextures(1, &m_TextureId);
        m_TextureId = 0;
    }
}
