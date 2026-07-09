#version 300 es

// Matching ecs::TransformComponent (alignas(16))
// float sw, sh, cw, ch;
// float rot, p1, p2, p3;
layout(std140) uniform TransformBlock {
    vec4 screen_cam; // x=sw, y=sh, z=cw, w=ch
    vec4 rotation_p; // x=rot, y=p1, z=p2, w=p3
} transform;

// Matching ecs::TimestampComponent (alignas(16))
// int32_t epochSeconds;
layout(std140) uniform TimestampBlock {
    ivec4 timeData; // x=epochSeconds
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

    float sw = transform.screen_cam.x;
    float sh = transform.screen_cam.y;
    float cw = transform.screen_cam.z;
    float ch = transform.screen_cam.w;
    float rot = transform.rotation_p.x;

    if (idx < 6) {
        // Background calculation
        type = 0.0;
        bool isRot = (abs(rot - 90.0) < 0.1 || abs(rot - 270.0) < 0.1);
        float acW = isRot ? ch : cw;
        float acH = isRot ? cw : ch;
        float sA = sw / sh;
        float cA = acW / acH;

        float cL = 0.0, cR = 1.0, cT = 0.0, cB = 1.0;
        if (sA > cA) {
            float r = cA / sA;
            float a = (1.0 - r) / 2.0;
            cT = a; cB = 1.0 - a;
        } else {
            float r = sA / cA;
            float a = (1.0 - r) / 2.0;
            cL = a; cR = 1.0 - a;
        }

        float uvs[8];
        if (abs(rot - 90.0) < 0.1) {
            uvs[0] = cL; uvs[1] = cB; uvs[2] = cR; uvs[3] = cB;
            uvs[4] = cL; uvs[5] = cT; uvs[6] = cR; uvs[7] = cT;
        } else {
            uvs[0] = cL; uvs[1] = cT; uvs[2] = cL; uvs[3] = cB;
            uvs[4] = cR; uvs[5] = cT; uvs[6] = cR; uvs[7] = cB;
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
        int cIdx = vIdx / 6;
        int tIdx = vIdx % 6;

        int epoch = timestamp.timeData.x;
        int h = (epoch / 3600) % 24;
        int m = (epoch / 60) % 60;
        int s = epoch % 60;

        int code;
        if (cIdx == 0) code = h / 10; else if (cIdx == 1) code = h % 10;
        else if (cIdx == 2) code = 10; else if (cIdx == 3) code = m / 10;
        else if (cIdx == 4) code = m % 10; else if (cIdx == 5) code = 10;
        else if (cIdx == 6) code = s / 10; else if (cIdx == 7) code = s % 10;

        float uS = float(code) / 11.0;
        float uE = float(code + 1) / 11.0;
        float xP = kTextPosX + float(cIdx) * kTextCharWidth;

        if (tIdx == 0) { pos = vec4(xP, kTextPosY, 0.0, 1.0); tex = vec2(uS, 0.0); }
        else if (tIdx == 1) { pos = vec4(xP, kTextPosY - kTextCharHeight, 0.0, 1.0); tex = vec2(uS, 1.0); }
        else if (tIdx == 2) { pos = vec4(xP + kTextCharWidth, kTextPosY, 0.0, 1.0); tex = vec2(uE, 0.0); }
        else if (tIdx == 3) { pos = vec4(xP, kTextPosY - kTextCharHeight, 0.0, 1.0); tex = vec2(uS, 1.0); }
        else if (tIdx == 4) { pos = vec4(xP + kTextCharWidth, kTextPosY - kTextCharHeight, 0.0, 1.0); tex = vec2(uE, 1.0); }
        else if (tIdx == 5) { pos = vec4(xP + kTextCharWidth, kTextPosY, 0.0, 1.0); tex = vec2(uE, 0.0); }
    }

    gl_Position = pos;
    vTexCoord = tex;
    vType = type;
}
