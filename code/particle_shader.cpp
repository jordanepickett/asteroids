#include "platform.h"
#include "light_uniforms.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <glm/gtc/type_ptr.hpp>

static void ParticleShaderInit(Program* program, const char* particleShader, const char* particleFragment) {
    
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &particleShader, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &particleFragment, NULL);
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
    //LightUniformsInit(program);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
