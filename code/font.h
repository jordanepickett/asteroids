#pragma once
#include <stdint.h>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "stb_truetype.h"

#define FIRST_CHAR 32
#define CHAR_COUNT 95   // 32..126

typedef struct {
    GLuint texture_id;
    int atlas_w;
    int atlas_h;
    float ascent, descent, line_gap;
    stbtt_bakedchar glyphs[96];
} Font;

inline bool LoadFont(Font* out, const char* ttf_path, float pixel_height) {
    FILE* f = fopen(ttf_path, "rb");
    if (!f) return false;
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char* ttf_buffer = (unsigned char*)malloc(size);
    fread(ttf_buffer, size, 1, f);
    fclose(f);

    const int ATLAS_W = 512;
    const int ATLAS_H = 512;
    unsigned char* atlas = (unsigned char*)calloc(ATLAS_W * ATLAS_H, 1);

    stbtt_BakeFontBitmap(ttf_buffer, 0, pixel_height, atlas, ATLAS_W, ATLAS_H, 32, 96, out->glyphs);

    glGenTextures(1, &out->texture_id);
    glBindTexture(GL_TEXTURE_2D, out->texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ATLAS_W, ATLAS_H, 0, GL_RED, GL_UNSIGNED_BYTE, atlas);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    out->atlas_w = ATLAS_W;
    out->atlas_h = ATLAS_H;

    free(atlas);
    free(ttf_buffer);
    return true;
}
