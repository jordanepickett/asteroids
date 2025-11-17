#include "font.h"
#include "platform.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

static void BasePostShaderInit(PlatformRenderer* renderer, const char* textVertexShader, const char* textFragmentShader) {

    // Generate framebuffer
    glGenFramebuffers(1, &renderer->frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, renderer->frameBuffer);

    // Generate and configure the render texture
    glGenTextures(1, &renderer->renderTexture);
    glBindTexture(GL_TEXTURE_2D, renderer->renderTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, INTERNAL_WIDTH, INTERNAL_HEIGHT, 
                 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Crisp pixels
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Attach texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                           GL_TEXTURE_2D, renderer->renderTexture, 0);

    // Check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("ERROR: Framebuffer is not complete!\n");
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &textVertexShader, NULL);
    glCompileShader(vertexShader);
    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &textFragmentShader, NULL);
    glCompileShader(fragmentShader);
    
    renderer->basePostProgram->program = glCreateProgram();
    glAttachShader(renderer->basePostProgram->program, vertexShader);
    glAttachShader(renderer->basePostProgram->program, fragmentShader);
    glLinkProgram(renderer->basePostProgram->program);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    
    // === CREATE QUAD GEOMETRY ===
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    
    glGenVertexArrays(1, &renderer->basePostProgram->vao);
    glGenBuffers(1, &renderer->basePostProgram->vbo);
    glBindVertexArray(renderer->basePostProgram->vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->basePostProgram->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    
    // TexCoord attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    glBindVertexArray(0);
}
