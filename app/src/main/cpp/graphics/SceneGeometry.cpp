#include "SceneGeometry.h"
#include <ctime>

SceneGeometry::SceneGeometry() : m_VboId(0) {}

SceneGeometry::~SceneGeometry() {
    Terminate();
}

void SceneGeometry::Create() {
    glGenBuffers(1, &m_VboId);
}

void SceneGeometry::Update() {
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[10];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, 10, "%H:%M:%S", timeinfo);

    std::vector<float> vertices;
    // 1. Background (2 triangles, type 0.0)
    float bg[] = {
        -1.0f,  1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
         1.0f, -1.0f, 0.0f, 1.0f,  1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
    };
    vertices.insert(vertices.end(), bg, bg + 42);

    // 2. Timestamp (8 chars, 2 triangles each, type 1.0)
    float xPos = kTextPosX;
    for (int i = 0; i < 8; ++i) {
        int glyphIdx = (buffer[i] == ':') ? 10 : (buffer[i] - '0');
        float uS = (float)glyphIdx / 11.0f;
        float uE = (float)(glyphIdx + 1) / 11.0f;
        float v[] = {
            xPos,                kTextPosY,                    0.0f, 1.0f,  uS, 0.0f, 1.0f,
            xPos,                kTextPosY - kTextCharHeight,  0.0f, 1.0f,  uS, 1.0f, 1.0f,
            xPos+kTextCharWidth, kTextPosY,                    0.0f, 1.0f,  uE, 0.0f, 1.0f,
            xPos,                kTextPosY - kTextCharHeight,  0.0f, 1.0f,  uS, 1.0f, 1.0f,
            xPos+kTextCharWidth, kTextPosY - kTextCharHeight,  0.0f, 1.0f,  uE, 1.0f, 1.0f,
            xPos+kTextCharWidth, kTextPosY,                    0.0f, 1.0f,  uE, 0.0f, 1.0f,
        };
        vertices.insert(vertices.end(), v, v + 42);
        xPos += kTextCharWidth;
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_VboId);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
}

void SceneGeometry::Terminate() {
    if (m_VboId != 0) {
        glDeleteBuffers(1, &m_VboId);
        m_VboId = 0;
    }
}
