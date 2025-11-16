#pragma once

#include "font.h"
#include "memory.h"
#include "platform_input.h"
#include <glm/glm.hpp>
#include <glad/glad.h>

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

static const char* textVertexShader =
"#version 330\n"
"uniform mat4 MVP;\n"
"in vec2 vPos;\n"
"in vec2 vTexCoord;\n"
"in vec4 vCol;\n"
"out vec2 texCoord;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    texCoord = vTexCoord;\n"
"    color = vCol;\n"
"}\n";

static const char* textFragmentShader =
"#version 330\n"
"uniform sampler2D fontTexture;\n"
"in vec2 texCoord;\n"
"in vec4 color;\n"
"out vec4 fragment;\n"
"void main()\n"
"{\n"
"    float alpha = texture(fontTexture, texCoord).r;\n"  // Sample RED channel
"    fragment = vec4(color.rgb, color.a * alpha);\n"     // Multiply color by alpha
"}\n";

struct Program {
    GLuint vbo, vao, vertexShader, fragmentShader, program;
    GLint mvpLocation, vposLocation, vcolLocation, location;
};

struct PlatformRenderer {
    GLuint vertexBuffer, vertexShader, fragmentShader, program, vao;
    GLint mvpLocation, vposLocation, vcolLocation;
    Program* textProgram;
    float ratio;
    int width, height;
    Font font;
};

struct PlatformMemory {
    MemoryArena permanent;
    MemoryArena transient;
};

struct PlatformFrame {
    float deltaTime;
    GameInput input;
};

struct PlatformAPI {
    void* (*AllocateMemory)(size_t size);
    void (*FreeMemory)(void* ptr);
    void (*Log)(const char* msg);
};

void PlatformInit(PlatformRenderer* renderer, const char* vertexShaderText, const char* fragementShaderText);

void PlatformRunGameLoop(PlatformAPI* api, PlatformRenderer *renderer, const char* vertexShaderText, const char* fragementShaderText);

void PlatformRender(PlatformRenderer *renderer, void* buffer, size_t size);
