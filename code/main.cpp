#include "platform.h"
#define M_PI 3.14159265358979323846f
#define GLAD_GL_IMPLEMENTATION
#define GLFW_INCLUDE_NONE
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

static const char* vertexShaderText =
"#version 330\n"
"uniform mat4 MVP;\n"
"in vec3 vCol;\n"
"in vec2 vPos;\n"
"out vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";
 
static const char* fragmentShaderText =
"#version 330\n"
"in vec3 color;\n"
"out vec4 fragment;\n"
"void main()\n"
"{\n"
"    fragment = vec4(color, 1.0);\n"
"}\n";


int main() {
    PlatformAPI platform = {0};
    PlatformRenderer renderer = {0};
    //PlatformInit(&renderer, vertexShaderText, fragmentShaderText);
    PlatformRunGameLoop(&platform, &renderer, vertexShaderText, fragmentShaderText);
}
