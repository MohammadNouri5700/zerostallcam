#version 300 es
#extension GL_OES_EGL_image_external_essl3 : require
precision mediump float;

uniform samplerExternalOES sCamera;
uniform sampler2D sFont;

in vec2 vTexCoord;
in float vType;
out vec4 fragColor;

void main() {
    if (vType < 0.5) {
        fragColor = texture(sCamera, vTexCoord);
    } else {
        vec4 fontColor = texture(sFont, vTexCoord);
        fragColor = vec4(1.0, 1.0, 0.0, fontColor.a);
    }
}
