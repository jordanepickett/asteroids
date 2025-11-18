#pragma once

#include "font.h"
#include "memory.h"
#include "platform_input.h"
#include <glm/glm.hpp>
#include <glad/glad.h>

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

static const int INTERNAL_WIDTH = 1280;
static const int INTERNAL_HEIGHT = 720;

static const char* vertexShaderText =
"#version 330\n"
"uniform mat4 MVP;\n"
"layout (location = 0) in vec2 vPos;\n"
"layout (location = 1) in vec4 vCol;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";
 
static const char* fragmentShaderText =
"#version 330\n"
"in vec4 color;\n"
"out vec4 fragment;\n"
"void main()\n"
"{\n"
"    fragment = vec4(color.rgb, color.a);\n"
"}\n";

static const char* textVertexShader =
"#version 330\n"
"uniform mat4 MVP;\n"
"layout (location = 0) in vec2 vPos;\n"
"layout (location = 1) in vec2 vTexCoord;\n"
"layout (location = 2) in vec4 vCol;\n"
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

static const char* basePostShader =
"#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos, 0.0, 1.0);\n"
"    TexCoord = aTexCoord;\n"
"}\n";

static const char* basePostFragment =
"#version 330 core\n"
"in vec2 TexCoord;\n"
"out vec4 FragColor;\n"
"uniform sampler2D screenTexture;\n"
"void main()\n"
"{\n"
"    FragColor = texture(screenTexture, TexCoord);\n"
"}\n";

static const char* basePostFragmentCRT = 
"#version 330 core\n"
"in vec2 TexCoord;\n"
"out vec4 FragColor;\n"
"uniform sampler2D screenTexture;\n"
"uniform float time;\n"
"void main()\n"
"{\n"
"    vec4 color = texture(screenTexture, TexCoord);\n"
"    float scanline = sin(TexCoord.y * 180.0 * 3.14159) * 0.1;\n"
"    color.rgb -= scanline;\n"
"    vec2 center = TexCoord - 0.5;\n"
"    float vignette = 1.0 - dot(center, center) * 0.5;\n"
"    color.rgb *= vignette;\n"
"    FragColor = color;\n"
"}\n";

struct Program {
    GLuint vbo, vao, vertexShader, fragmentShader, program;
    GLint mvpLocation, vposLocation, vcolLocation, location;
};

struct PlatformRenderer {
    GLuint vertexVAO,vertexVBO, textVAO, textVBO, screenVAO, screenVBO;
    Program* textProgram;
    Program* vertexProgram;
    Program* basePostProgram;
    GLuint renderTexture, frameBuffer, postProcessingTexture, bloomTexture;
    float ratio;
    int width, height;
    Font fontUI;
    Font fontTitle;
    Font fontDebug;
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
