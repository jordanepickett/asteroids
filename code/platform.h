#pragma once

#include "memory.h"
#include "platform_input.h"
#include <glm/glm.hpp>
#include <glad/glad.h>

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

struct PlatformRenderer {
    GLuint vertexBuffer, vertexShader, fragmentShader, program, vao;
    GLint mvpLocation, vposLocation, vcolLocation;
    float ratio;
    int width, height;
};

struct PlatformMemory {
    MemoryArena permantent;
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
