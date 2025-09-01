#include "game.h"
#include "memory.h"
#include "render_commands.h"
#include "platform.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void PlatformInit(PlatformRenderer *renderer, const char* vertexShaderText, const char* fragmentShaderText) {

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        // Error handling
        return;
    }


    glGenBuffers(1, &renderer->vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vertexBuffer);

    renderer->vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(renderer->vertexShader, 1, &vertexShaderText, NULL);
    glCompileShader(renderer->vertexShader);

    renderer->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(renderer->fragmentShader, 1, &fragmentShaderText, NULL);
    glCompileShader(renderer->fragmentShader);

    renderer->program = glCreateProgram();
    glAttachShader(renderer->program, renderer->vertexShader);
    glAttachShader(renderer->program, renderer->fragmentShader);
    glLinkProgram(renderer->program);

    renderer->mvpLocation = glGetUniformLocation(renderer->program, "MVP");
    renderer->vposLocation = glGetAttribLocation(renderer->program, "vPos");
    renderer->vcolLocation = glGetAttribLocation(renderer->program, "vCol");

    glGenVertexArrays(1, &renderer->vao);
    glBindVertexArray(renderer->vao);

    // position attribute
    glEnableVertexAttribArray(renderer->vposLocation);
    glVertexAttribPointer(
        renderer->vposLocation,
        2, GL_FLOAT, GL_FALSE,
        sizeof(Vertex), 
        (void*)offsetof(Vertex, pos)
    );

    // color attribute
    glEnableVertexAttribArray(renderer->vcolLocation);
    glVertexAttribPointer(
        renderer->vcolLocation,
        3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, color)
    );

    // unbind VAO (keeps state stored in VAO)
    glBindVertexArray(0);
}

void PlatformRunGameLoop(PlatformAPI *api,
                         PlatformRenderer *renderer,
                         const char* vertexShaderText, 
                         const char* fragmentShaderText
                         ) {
    if(!glfwInit()) {
        return;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Asteroids", 0, 0);

    if (!window)
    {
        glfwTerminate();
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSwapInterval(1);

    PlatformInit(renderer, vertexShaderText, fragmentShaderText);

    const size_t PERM_SIZE = 64 * 1024 * 1024;
    const size_t TRANS_SIZE = 64 * 1024 * 1024;
    
    void* permMem = malloc(PERM_SIZE);
    void* transMem = malloc(TRANS_SIZE);

    PlatformMemory memory = {};
    ArenaInit(&memory.permantent, permMem, PERM_SIZE);
    ArenaInit(&memory.transient, transMem, TRANS_SIZE);


    GameState game = {};
    GameInit(&game, api, &memory);

    double lastFrameTime = 0.0;

    while(!glfwWindowShouldClose(window)) {
        ArenaReset(&memory.transient);

        PlatformFrame frame = {};
        double currentFrameTime = glfwGetTime();
        double deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        frame.deltaTime = deltaTime;

        glfwGetFramebufferSize(window, &renderer->width, &renderer->height);
        renderer->ratio = renderer->width / (float) renderer->height;

        GameUpdate(&game, &frame, &memory);
        GameRender(&game, &memory);

        PlatformRender(renderer, game.commands, game.renderCommandsCount);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}


void PlatformRender(PlatformRenderer* renderer, void* buffer, size_t size) {

    uint8_t* ptr = (uint8_t*)buffer;
    uint8_t* end = ptr + size;

    glViewport(0, 0, renderer->width, renderer->height);

    while (ptr < end) {
        RenderCommandHeader* header = (RenderCommandHeader*)ptr;

        switch (header->type) {
            case RENDER_CMD_CLEAR: {
                auto* c = (RenderCommandClear*)ptr;
                glClearColor(c->color.r, c->color.g, c->color.b, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            } break;

            case RENDER_CMD_DRAW_TRIANGLES: {

                auto* d = (RenderCommandDrawTriangles*)ptr;
                void* verts = (uint8_t*)d + sizeof(RenderCommandDrawTriangles);
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
                glm::mat4 proj = glm::ortho(-renderer->ratio, renderer->ratio, -1.0f, 1.0f, 1.0f, -1.0f);
                glm::mat4 mvp = proj * model;

                glUseProgram(renderer->program);
                glBindBuffer(GL_ARRAY_BUFFER, renderer->vertexBuffer);

                glBufferData(GL_ARRAY_BUFFER, d->vertexCount * sizeof(Vertex), verts, GL_DYNAMIC_DRAW);
                glUniformMatrix4fv(renderer->mvpLocation, 1, GL_FALSE, (const GLfloat*)&mvp);

                glBindVertexArray(renderer->vao);
                glDrawArrays(GL_TRIANGLES, 0, d->vertexCount);

                glBindVertexArray(0);
            } break;
            case RENDER_CMD_DRAW_TEXT: {
                /*
                auto* t = (RenderCommandDrawText*)ptr;
                const char* str = (char*)t + sizeof(RenderCommandDrawText);
                RenderTextWithFontAtlas(str, t->position, t->color);
                    */
            } break;
        }

        ptr += header->size; // move to next command
    }
}
