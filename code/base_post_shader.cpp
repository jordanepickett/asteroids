#include "font.h"
#include "platform.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

static void BasePostShaderInit(PlatformRenderer* renderer, Program* program, const char* textVertexShader, const char* textFragmentShader) {

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
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
