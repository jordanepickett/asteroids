#include "platform.h"
#include "entity.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

static void VertexShaderInit(PlatformRenderer* renderer, const char* textVertexShader, const char* textFragmentShader) {
    glGenBuffers(1, &renderer->vertexProgram->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vertexProgram->vbo);

    renderer->vertexProgram->vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(renderer->vertexProgram->vertexShader, 1, &vertexShaderText, NULL);
    glCompileShader(renderer->vertexProgram->vertexShader);

    renderer->vertexProgram->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(renderer->vertexProgram->fragmentShader, 1, &fragmentShaderText, NULL);
    glCompileShader(renderer->vertexProgram->fragmentShader);

    renderer->vertexProgram->program = glCreateProgram();
    glAttachShader(renderer->vertexProgram->program, renderer->vertexProgram->vertexShader);
    glAttachShader(renderer->vertexProgram->program, renderer->vertexProgram->fragmentShader);
    glLinkProgram(renderer->vertexProgram->program);

    renderer->vertexProgram->mvpLocation = glGetUniformLocation(renderer->vertexProgram->program, "MVP");
    renderer->vertexProgram->vposLocation = glGetAttribLocation(renderer->vertexProgram->program, "vPos");
    renderer->vertexProgram->vcolLocation = glGetAttribLocation(renderer->vertexProgram->program, "vCol");

    glGenVertexArrays(1, &renderer->vertexProgram->vao);
    glBindVertexArray(renderer->vertexProgram->vao);

    // position attribute
    glEnableVertexAttribArray(renderer->vertexProgram->vposLocation);
    glVertexAttribPointer(
        renderer->vertexProgram->vposLocation,
        2, GL_FLOAT, GL_FALSE,
        sizeof(Vertex), 
        (void*)offsetof(Vertex, position)
    );

    // color attribute
    glEnableVertexAttribArray(renderer->vertexProgram->vcolLocation);
    glVertexAttribPointer(
        renderer->vertexProgram->vcolLocation,
        3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, color)
    );

    // unbind VAO (keeps state stored in VAO)
    glBindVertexArray(0);
}
