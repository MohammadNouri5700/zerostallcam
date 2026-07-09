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
    float xPos = -0.9f;
    float yPos = 0.8f;
    float charW = 0.05f;
    float charH = 0.1f;
    for (int i = 0; i < 8; ++i) {
        int glyphIdx = (buffer[i] == ':') ? 10 : (buffer[i] - '0');
        float uS = (glyphIdx * 10.0f) / 128.0f;
        float uE = (glyphIdx * 10.0f + 8.0f) / 128.0f;
        float v[] = {
            xPos,       yPos,       0.0f, 1.0f,  uS, 0.0f, 1.0f,
            xPos,       yPos-charH, 0.0f, 1.0f,  uS, 1.0f, 1.0f,
            xPos+charW, yPos,       0.0f, 1.0f,  uE, 0.0f, 1.0f,
            xPos,       yPos-charH, 0.0f, 1.0f,  uS, 1.0f, 1.0f,
            xPos+charW, yPos-charH, 0.0f, 1.0f,  uE, 1.0f, 1.0f,
            xPos+charW, yPos,       0.0f, 1.0f,  uE, 0.0f, 1.0f,
        };
        vertices.insert(vertices.end(), v, v + 42);
        xPos += charW;
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
