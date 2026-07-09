#version 300 es

layout(std140, binding = 1) uniform TransformBlock {
    int screenWidth;
    int screenHeight;
    int cameraWidth;
    int cameraHeight;
    int rotationDegrees;
} transform;

layout(std140, binding = 2) uniform TimestampBlock {
    int hours;
    int minutes;
    int seconds;
} timestamp;

out vec2 vTexCoord;
out float vType;

const float kTextPosX = -0.99;
const float kTextPosY = 0.99;
const float kTextCharWidth = 0.06;
const float kTextCharHeight = 0.05;

void main() {
    int idx = gl_VertexID;
    vec4 pos;
    vec2 tex;
    float type;

    if (idx < 6) {
        // Background calculation
        type = 0.0;
        bool isRotated = (transform.rotationDegrees == 90 || transform.rotationDegrees == 270);
        float activeCamW = isRotated ? float(transform.cameraHeight) : float(transform.cameraWidth);
        float activeCamH = isRotated ? float(transform.cameraWidth) : float(transform.cameraHeight);
        float screenAspect = float(transform.screenWidth) / float(transform.screenHeight);
        float camAspect = activeCamW / activeCamH;

        float cropL = 0.0, cropR = 1.0, cropT = 0.0, cropB = 1.0;
        if (screenAspect > camAspect) {
            float ratio = camAspect / screenAspect;
            float amt = (1.0 - ratio) / 2.0;
            cropT = amt; cropB = 1.0 - amt;
        } else {
            float ratio = screenAspect / camAspect;
            float amt = (1.0 - ratio) / 2.0;
            cropL = amt; cropR = 1.0 - amt;
        }

        float uvs[8];
        if (transform.rotationDegrees == 90) {
            uvs[0] = cropL; uvs[1] = cropB; uvs[2] = cropR; uvs[3] = cropB;
            uvs[4] = cropL; uvs[5] = cropT; uvs[6] = cropR; uvs[7] = cropT;
        } else {
            uvs[0] = cropL; uvs[1] = cropT; uvs[2] = cropL; uvs[3] = cropB;
            uvs[4] = cropR; uvs[5] = cropT; uvs[6] = cropR; uvs[7] = cropB;
        }

        if (idx == 0) { pos = vec4(-1.0,  1.0, 0.0, 1.0); tex = vec2(uvs[0], uvs[1]); }
        else if (idx == 1) { pos = vec4(-1.0, -1.0, 0.0, 1.0); tex = vec2(uvs[2], uvs[3]); }
        else if (idx == 2) { pos = vec4( 1.0,  1.0, 0.0, 1.0); tex = vec2(uvs[4], uvs[5]); }
        else if (idx == 3) { pos = vec4(-1.0, -1.0, 0.0, 1.0); tex = vec2(uvs[2], uvs[3]); }
        else if (idx == 4) { pos = vec4( 1.0, -1.0, 0.0, 1.0); tex = vec2(uvs[6], uvs[7]); }
        else if (idx == 5) { pos = vec4( 1.0,  1.0, 0.0, 1.0); tex = vec2(uvs[4], uvs[5]); }
    } else {
        // Timestamp calculation
        type = 1.0;
        int vIdx = idx - 6;
        int charIdx = vIdx / 6;
        int triIdx = vIdx % 6;

        int charCode;
        if (charIdx == 0) charCode = timestamp.hours / 10;
        else if (charIdx == 1) charCode = timestamp.hours % 10;
        else if (charIdx == 2) charCode = 10;
        else if (charIdx == 3) charCode = timestamp.minutes / 10;
        else if (charIdx == 4) charCode = timestamp.minutes % 10;
        else if (charIdx == 5) charCode = 10;
        else if (charIdx == 6) charCode = timestamp.seconds / 10;
        else if (charIdx == 7) charCode = timestamp.seconds % 10;

        float uS = float(charCode) / 11.0;
        float uE = float(charCode + 1) / 11.0;
        float xPos = kTextPosX + float(charIdx) * kTextCharWidth;

        if (triIdx == 0) { pos = vec4(xPos, kTextPosY, 0.0, 1.0); tex = vec2(uS, 0.0); }
        else if (triIdx == 1) { pos = vec4(xPos, kTextPosY - kTextCharHeight, 0.0, 1.0); tex = vec2(uS, 1.0); }
        else if (triIdx == 2) { pos = vec4(xPos + kTextCharWidth, kTextPosY, 0.0, 1.0); tex = vec2(uE, 0.0); }
        else if (triIdx == 3) { pos = vec4(xPos, kTextPosY - kTextCharHeight, 0.0, 1.0); tex = vec2(uS, 1.0); }
        else if (triIdx == 4) { pos = vec4(xPos + kTextCharWidth, kTextPosY - kTextCharHeight, 0.0, 1.0); tex = vec2(uE, 1.0); }
        else if (triIdx == 5) { pos = vec4(xPos + kTextCharWidth, kTextPosY, 0.0, 1.0); tex = vec2(uE, 0.0); }
    }

    gl_Position = pos;
    vTexCoord = tex;
    vType = type;
}
