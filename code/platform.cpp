#include "text_shader.cpp"
#include "vertex_shader.cpp"
#include "entity.h"
#include "font.h"
#include "game.h"
#include "memory.h"
#include "render_commands.h"
#include "platform.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <glm/gtc/type_ptr.hpp>

#define PushStruct(arena, type) (type *)PushSize_(arena, sizeof(type))

inline void* PushSize_(MemoryArena* arena, size_t size) {
    assert(arena->used + size <= arena->size);
    void* ptr = (uint8_t*)arena->base + arena->used;
    arena->used += size;
    return ptr;
}

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void ProcessInputDigitalButton(
    unsigned char buttonState,
    ButtonState *oldState,
    ButtonState *newState
) {

    newState->endedDown = buttonState == GLFW_PRESS;
    newState->halfTransitionCount = (oldState->endedDown != newState->endedDown) ? 1 : 0;
}

static float ApplyDeadzone(float value, float deadzone) {
    if (fabs(value) < deadzone) {
        return 0.0f;
    }

    float sign = (value > 0) ? 1.0f : -1.0f;
    float adjusted = (fabs(value) - deadzone) / (1.0f - deadzone);
    return adjusted * sign;
}

void PlatformInit(PlatformRenderer *renderer, PlatformMemory* memory) {

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        // Error handling
        return;
    }

    //Vertex Shader
    Program* vertexProgram = (Program*)ArenaAlloc(&memory->permanent, sizeof(Program));
    renderer->vertexProgram = vertexProgram;
    VertexShaderInit(renderer, vertexShaderText, fragmentShaderText);

    //Texture Shader
    Program* textProgram = (Program*)ArenaAlloc(&memory->permanent, sizeof(Program));
    renderer->textProgram = textProgram;
    TextShaderInit(renderer, textVertexShader, textFragmentShader);

    {
        // Load font title font
        Font font;
        bool isLoaded = LoadFont(&font, "pixel.ttf", 32.0f);
        if(isLoaded) {
            printf("Font loaded.\n");
            renderer->fontTitle = font;
        } else {
            printf("Font not loaded.\n");
        }
    }
    {
        // Load font title font
        Font font;
        bool isLoaded = LoadFont(&font, "pixel.ttf", 16.0f);
        if(isLoaded) {
            printf("Font loaded.\n");
            renderer->fontUI = font;
        } else {
            printf("Font not loaded.\n");
        }
    }
    {
        // Load font title font
        Font font;
        bool isLoaded = LoadFont(&font, "pixel.ttf", 12.0f);
        if(isLoaded) {
            printf("Font loaded.\n");
            renderer->fontDebug = font;
        } else {
            printf("Font not loaded.\n");
        }
    }
}

void PlatformRunGameLoop(PlatformAPI *api,
                         PlatformRenderer *renderer,
                         const char* vertexShaderText, 
                         const char* fragmentShaderText
                         ) {
    if(!glfwInit()) {
        return;
    }

    GLFWwindow* window = glfwCreateWindow(640, 360, "Asteroids", 0, 0);

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


    const size_t PERM_SIZE = 64 * 1024 * 1024;
    const size_t TRANS_SIZE = 64 * 1024 * 1024;
    
    void* permMem = malloc(PERM_SIZE);
    void* transMem = malloc(TRANS_SIZE);

    PlatformMemory memory = {};
    ArenaInit(&memory.permanent, permMem, PERM_SIZE);
    ArenaInit(&memory.transient, transMem, TRANS_SIZE);


    PlatformInit(renderer, &memory);
    GameState* game = (GameState*)ArenaAlloc(&memory.permanent, sizeof(GameState));
    GameInit(game, api, &memory);


    double lastFrameTime = 0.0;

    GameInput input[2] = {};
    GameInput *newInput = &input[0];
    GameInput *oldInput = &input[1];

    while(!glfwWindowShouldClose(window)) {
        ArenaReset(&memory.transient);

        PlatformFrame frame = {};
        double currentFrameTime = glfwGetTime();
        double deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        frame.deltaTime = deltaTime;

        //TODO Keyboard input
        GLFWgamepadstate state;
        if(glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) {
            //TODO Temp getting first controller
            //ControllerInput *oldController = &frame.input.controllers[0];
            ControllerInput *oldController = &oldInput->controllers[0];
            //ControllerInput *newController = &frame.input.controllers[0];
            ControllerInput *newController = &newInput->controllers[0];

            newController->stickAverageX = ApplyDeadzone(state.axes[GLFW_GAMEPAD_AXIS_LEFT_X], 0.25f);
            newController->stickAverageY = ApplyDeadzone(state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y], 0.25f);

            float threshold = 0.5f;
            ProcessInputDigitalButton(
                (newController->stickAverageX < -threshold) ? 1 : 0,
                &oldController->moveLeft,
                &newController->moveLeft
            );
            ProcessInputDigitalButton(
                (newController->stickAverageX > threshold) ? 1 : 0,
                &oldController->moveRight,
                &newController->moveRight
            );
            ProcessInputDigitalButton(
                (newController->stickAverageY < -threshold) ? 1 : 0,
                &oldController->moveUp,
                &newController->moveUp
            );
            ProcessInputDigitalButton(
                (newController->stickAverageY > threshold) ? 1 : 0,
                &oldController->moveDown,
                &newController->moveDown
            );

            ProcessInputDigitalButton(
                state.buttons[GLFW_GAMEPAD_BUTTON_A],
                &oldController->actionDown,
                &newController->actionDown
            );
            ProcessInputDigitalButton(
                state.buttons[GLFW_GAMEPAD_BUTTON_B],
                &oldController->actionRight,
                &newController->actionRight
            );
            ProcessInputDigitalButton(
                state.buttons[GLFW_GAMEPAD_BUTTON_Y],
                &oldController->actionUp,
                &newController->actionUp
            );
            ProcessInputDigitalButton(
                state.buttons[GLFW_GAMEPAD_BUTTON_X],
                &oldController->actionLeft,
                &newController->actionLeft
            );
            ProcessInputDigitalButton(
                state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER],
                &oldController->rightShoulder,
                &newController->rightShoulder
            );
            ProcessInputDigitalButton(
                state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER],
                &oldController->leftShoulder,
                &newController->leftShoulder
            );

            //lx = applyDeadzone(state.axes[GLFW_GAMEPAD_AXIS_LEFT_X], 0.25f);
            //ly = applyDeadzone(state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y], 0.25f);

            //vec2 inputAxis = {lx, ly};

            //player.targetVelocity[0] = inputAxis[0] * player.maxSpeed;
            //player.targetVelocity[1] = inputAxis[1] * player.maxSpeed;

            frame.input.controllers[0] = newInput->controllers[0];
        }

        glfwGetFramebufferSize(window, &renderer->width, &renderer->height);
        renderer->ratio = renderer->width / (float) renderer->height;

        GameUpdate(game, &frame, &memory);
        GameRender(game, &memory, &frame);

        PlatformRender(renderer, game->commands, game->renderCommandsCount);

        GameInput *temp = newInput;
        newInput = oldInput;
        oldInput = temp;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

static void DrawQuadTextured(
    PlatformRenderer* renderer,
    float x0, float y0,
    float x1, float y1,
    float s0, float t0,
    float s1, float t1,
    const float color[4]
)
{
    TextVertex verts[6] = {
        {{ x0, y0 }, s0, t0, { color[0], color[1], color[2], color[3] }},
        {{x1, y0 }, s1, t0, { color[0], color[1], color[2], color[3] }},
        {{x1, y1 }, s1, t1, { color[0], color[1], color[2], color[3] }},
        
        {{ x0, y0 }, s0, t0, { color[0], color[1], color[2], color[3] }},
        {{ x1, y1 }, s1, t1, { color[0], color[1], color[2], color[3] }},
        {{ x0, y1 }, s0, t1, { color[0], color[1], color[2], color[3] }}
    };
    
    glBindBuffer(GL_ARRAY_BUFFER, renderer->textProgram->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6);}

static void RenderText(PlatformRenderer *renderer, const char* text, glm::vec2 pos, glm::vec4 color, glm::mat4 mvp) {

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    Font* usedFont = &renderer->fontDebug;
    glUseProgram(renderer->textProgram->program);
    glBindVertexArray(renderer->textProgram->vao);
    glBindTexture(GL_TEXTURE_2D, usedFont->texture_id);
    
    glUniformMatrix4fv(renderer->textProgram->mvpLocation, 1, GL_FALSE, &mvp[0][0]);
    glUniform1i(renderer->textProgram->location, 0); // Use texture unit 0

    float x = pos.x;
    float y = pos.y;
    const float col[4] = {color.r, color.g, color.b, color.a};

    for (int i = 0; text[i]; i++) {
        char c = text[i];
        if (c < 32 || c > 126) continue;
        
        const stbtt_bakedchar& g = usedFont->glyphs[c - 32];
        
        float x0 = x + g.xoff;
        float y0 = y + g.yoff;
        float x1 = x0 + (g.x1 - g.x0);
        float y1 = y0 + (g.y1 - g.y0);

        float s0 = g.x0 / (float)usedFont->atlas_w;
        float t0 = g.y0 / (float)usedFont->atlas_h;
        float s1 = g.x1 / (float)usedFont->atlas_w;
        float t1 = g.y1 / (float)usedFont->atlas_h;

        DrawQuadTextured(renderer, x0, y0, x1, y1, s0, t0, s1, t1, col);

        x += g.xadvance;
    }
    
    glBindVertexArray(0);
    glDisable(GL_BLEND);
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
                model = glm::translate(model, glm::vec3(-d->pos.x, d->pos.y, 0.0f));
                //model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
                glm::vec2 right(d->rotation.y, -d->rotation.x);  // perpendicular vector
                model[0][0] = right.x;  model[1][0] = right.y;
                model[0][1] = d->rotation.x; model[1][1] = d->rotation.y;
                //glm::mat4 proj = glm::ortho(-renderer->ratio, renderer->ratio, -1.0f, 1.0f, 1.0f, -1.0f);
                glm::mat4 mvp = d->mvp * model;

                glUseProgram(renderer->vertexProgram->program);
                glBindBuffer(GL_ARRAY_BUFFER, renderer->vertexProgram->vbo);

                glBufferData(GL_ARRAY_BUFFER, d->vertexCount * sizeof(Vertex), verts, GL_DYNAMIC_DRAW);
                glUniformMatrix4fv(renderer->vertexProgram->mvpLocation, 1, GL_FALSE, (const GLfloat*)&mvp);

                glBindVertexArray(renderer->vertexProgram->vao);
                glDrawArrays(GL_TRIANGLES, 0, d->vertexCount);

                glBindVertexArray(0);
            } break;
            case RENDER_CMD_DRAW_LOOP: {

                auto* d = (RenderCommandDrawTriangles*)ptr;
                void* verts = (uint8_t*)d + sizeof(RenderCommandDrawTriangles);
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(-d->pos.x, d->pos.y, 0.0f));
                //model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
                //glm::mat4 proj = glm::ortho(-renderer->ratio, renderer->ratio, -1.0f, 1.0f, 1.0f, -1.0f);
                glm::mat4 mvp = d->mvp * model;

                glUseProgram(renderer->vertexProgram->program);
                glBindBuffer(GL_ARRAY_BUFFER, renderer->vertexProgram->vbo);

                glBufferData(GL_ARRAY_BUFFER, d->vertexCount * sizeof(Vertex), verts, GL_DYNAMIC_DRAW);
                glUniformMatrix4fv(renderer->vertexProgram->mvpLocation, 1, GL_FALSE, (const GLfloat*)&mvp);

                glBindVertexArray(renderer->vertexProgram->vao);
                glDrawArrays(GL_LINE_LOOP, 0, d->vertexCount);

                glBindVertexArray(0);
            } break;
            case RENDER_CMD_DRAW_TEXT: {
                auto* t = (RenderCommandDrawText*)ptr;
                const char* str = (char*)t + sizeof(RenderCommandDrawText);
                glm::mat4 textMVP = glm::ortho(0.0f, (float)renderer->width, 
                                               (float)renderer->height, 0.0f, 
                                               -1.0f, 1.0f);
                RenderText(renderer, str, t->position, t->color, textMVP);
            } break;
        }

        ptr += header->size; // move to next command
    }
}

