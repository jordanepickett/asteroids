#include "platform.h"
#include "entity.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

void TextShaderInit(PlatformRenderer* renderer, const char* textVertexShader, const char* textFragmentShader) {
    // Compile shaders (similar to your existing shader compilation)
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &textVertexShader, NULL);
    glCompileShader(vs);
    
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &textFragmentShader, NULL);
    glCompileShader(fs);
    
    renderer->textProgram->program = glCreateProgram();
    glAttachShader(renderer->textProgram->program, vs);
    glAttachShader(renderer->textProgram->program, fs);
    glLinkProgram(renderer->textProgram->program);
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    // Get uniform locations
    renderer->textProgram->mvpLocation = glGetUniformLocation(renderer->textProgram->program, "MVP");
    renderer->textProgram->location = glGetUniformLocation(renderer->textProgram->program, "fontTexture");
    
    // Setup VAO for text rendering
    glGenVertexArrays(1, &renderer->textProgram->vao);
    glGenBuffers(1, &renderer->textProgram->vbo);
    
    glBindVertexArray(renderer->textProgram->vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->textProgram->vbo);
    
    // Position attribute
    GLint vPosLoc = glGetAttribLocation(renderer->textProgram->program, "vPos");
    glEnableVertexAttribArray(vPosLoc);
    glVertexAttribPointer(vPosLoc, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)0);
    
    // TexCoord attribute
    GLint vTexLoc = glGetAttribLocation(renderer->textProgram->program, "vTexCoord");
    glEnableVertexAttribArray(vTexLoc);
    glVertexAttribPointer(vTexLoc, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)(2 * sizeof(float)));
    
    // Color attribute
    GLint vColLoc = glGetAttribLocation(renderer->textProgram->program, "vCol");
    glEnableVertexAttribArray(vColLoc);
    glVertexAttribPointer(vColLoc, 4, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)(4 * sizeof(float)));
    
    glBindVertexArray(0);
}
