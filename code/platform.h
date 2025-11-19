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
"out vec2 texCoords;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos, 0.0, 1.0);\n"
"    texCoords = aTexCoord;\n"
"}\n";

static const char* basePostFragment =
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 texCoords;\n"
"uniform sampler2D screenTexture;\n"
"uniform sampler2D bloomTexture;\n"
"uniform float gamma;\n"
"void main()\n"
"{\n"
"    vec3 fragment = texture(screenTexture, texCoords).rgb;\n"
"    vec3 bloom = texture(bloomTexture, texCoords).rgb;\n"
"    vec3 color = fragment + bloom *0.8;\n"
"    float exposure = 0.8f;\n"
"    vec3 toneMapped = vec3(1.0f) - exp(-color * exposure);\n"
"    FragColor.rgb = pow(toneMapped, vec3(1.0f / gamma));\n"
"}\n";

static const char* basePostFragmentCRT = 
"#version 330 core\n"
"in vec2 texCoords;\n"
"out vec4 FragColor;\n"
"uniform sampler2D screenTexture;\n"
"uniform float time;\n"
"void main()\n"
"{\n"
"    vec4 color = texture(screenTexture, texCoords);\n"
"    float scanline = sin(texCoords.y * 180.0 * 3.14159) * 0.1;\n"
"    color.rgb -= scanline;\n"
"    vec2 center = texCoords - 0.5;\n"
"    float vignette = 1.0 - dot(center, center) * 0.5;\n"
"    color.rgb *= vignette;\n"
"    FragColor = color;\n"
"}\n";

static const char* blurFragment = 
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 texCoords;\n"
"uniform sampler2D screenTexture;\n"
"uniform bool horizontal;\n"
"const float weights[5] = float[](\n"
"    0.227027,  \n"
"    0.194594, \n" 
"    0.121622, \n"
"    0.054054, \n" 
"    0.016216  \n"
");\n"
"\n"
"void main()\n"
"{\n"
"    vec2 texel = 1.0 / textureSize(screenTexture, 0);\n"
"    vec3 result = texture(screenTexture, texCoords).rgb * weights[0];\n"
"\n"
"    if (horizontal) {\n"
"        for (int i = 1; i < 5; i++) {\n"
"            result += texture(screenTexture, texCoords + vec2(texel.x * i, 0)).rgb * weights[i];\n"
"            result += texture(screenTexture, texCoords - vec2(texel.x * i, 0)).rgb * weights[i];\n"
"        }\n"
"    } else {\n"
"        for (int i = 1; i < 5; i++) {\n"
"            result += texture(screenTexture, texCoords + vec2(0, texel.y * i)).rgb * weights[i];\n"
"            result += texture(screenTexture, texCoords - vec2(0, texel.y * i)).rgb * weights[i];\n"
"        }\n"
"    }\n"
"\n"
"    FragColor = vec4(result, 1.0);\n"
"}\n";

struct Program {
    GLuint program;
};

struct PlatformRenderer {
    //VAO/VBO
    GLuint vertexVAO,vertexVBO, textVAO, textVBO, screenVAO, screenVBO;

    // Textures
    GLuint renderTexture, frameBuffer, postProcessingTexture, bloomTexture;
    GLuint pingPongBuffer[2], pingPongTexture[2];
    
    // Programs
    Program* textProgram;
    Program* vertexProgram;
    Program* basePostProgram;
    Program* blurProgram;
    
    // Screen Width/Height
    float ratio;
    int width, height;

    // Fonts
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
