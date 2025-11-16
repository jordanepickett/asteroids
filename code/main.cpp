#include "platform.h"
#define M_PI 3.14159265358979323846f
#define GLAD_GL_IMPLEMENTATION
#define GLFW_INCLUDE_NONE
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

int main() {
    PlatformAPI platform = {0};
    PlatformRenderer renderer = {0};
    //PlatformInit(&renderer, vertexShaderText, fragmentShaderText);
    PlatformRunGameLoop(&platform, &renderer, vertexShaderText, fragmentShaderText);
}
