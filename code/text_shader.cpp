#include "font.h"
#include "platform.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

static void TextShaderInit(PlatformRenderer* renderer, Program* program, const char* textVertexShader, const char* textFragmentShader) {
    // Compile shaders (similar to your existing shader compilation)
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &textVertexShader, NULL);
    glCompileShader(vertexShader);
    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &textFragmentShader, NULL);
    glCompileShader(fragmentShader);
    
    program->program = glCreateProgram();
    glAttachShader(program->program, vertexShader);
    glAttachShader(program->program, fragmentShader);
    glLinkProgram(program->program);

    for (int i = 0; i < U_COUNT; i++) {
        program->uniformLocations[i] = -1;
    }

    for (int i = 0; i < sizeof(UNIFORM_TABLE)/sizeof(UniformDef); i++)
    {
        auto entry = UNIFORM_TABLE[i];
        program->uniformLocations[entry.id] =
            glGetUniformLocation(program->program, entry.name);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
