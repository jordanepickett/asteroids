#include "defs.h"
#include "systems.h"
#include "text_shader.cpp"
#include "vertex_shader.cpp"
#include "particle_shader.cpp"
#include "base_post_shader.cpp"
#include "blur_shader.cpp"
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

    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    // Vertex VAO/VBO
    glGenVertexArrays(1, &renderer->vertexVAO);
    glBindVertexArray(renderer->vertexVAO);

    glGenBuffers(1, &renderer->vertexVBO);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vertexVBO);

    // Set attribute layout for this VAO
    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, position));

    glEnableVertexAttribArray(1); // color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, color));

    glEnableVertexAttribArray(2); // normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, normal));

    glBindVertexArray(0);

    // Particle VAO/VBO
    glGenVertexArrays(1, &renderer->particleVAO);
    glBindVertexArray(renderer->particleVAO);

    glGenBuffers(1, &renderer->particleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->particleVBO);

    // Set attribute layout for this VAO
    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex),
                          (void*)offsetof(ParticleVertex, position));

    glEnableVertexAttribArray(1); // color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex),
                          (void*)offsetof(ParticleVertex, color));

    glBindVertexArray(0);

    // Text VAO/VBO
    glGenVertexArrays(1, &renderer->textVAO);
    glBindVertexArray(renderer->textVAO);

    glGenBuffers(1, &renderer->textVBO);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->textVBO);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex),
                          (void*)offsetof(TextVertex, position));

    // uv
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex),
                          (void*)offsetof(TextVertex, uv));

    // color
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(TextVertex),
                          (void*)offsetof(TextVertex, color));

    glBindVertexArray(0);

    glGenVertexArrays(1, &renderer->screenVAO);
    glBindVertexArray(renderer->screenVAO);

    glGenBuffers(1, &renderer->screenVBO);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    
    // TexCoord attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    glBindVertexArray(0);

    //Vertex Shader
    Program* vertexProgram = (Program*)ArenaAlloc(&memory->permanent, sizeof(Program));
    renderer->vertexProgram = vertexProgram;
    VertexShaderInit(renderer, renderer->vertexProgram, vertexShaderText, fragmentShaderText);

    //Particle Shader
    Program* particleProgram = (Program*)ArenaAlloc(&memory->permanent, sizeof(Program));
    renderer->particleProgram = particleProgram;
    ParticleShaderInit(renderer->particleProgram, particleShader, particleFragment);

    //Texture Shader
    Program* textProgram = (Program*)ArenaAlloc(&memory->permanent, sizeof(Program));
    renderer->textProgram = textProgram;
    TextShaderInit(renderer, renderer->textProgram, textVertexShader, textFragmentShader);

    // Base Post Shader
    Program* basePostProgram = (Program*)ArenaAlloc(&memory->permanent, sizeof(Program));
    renderer->basePostProgram = basePostProgram;
    BasePostShaderInit(renderer, renderer->basePostProgram, basePostShader, basePostFragment);

    // Blur Shader
    Program* blurProgram = (Program*)ArenaAlloc(&memory->permanent, sizeof(Program));
    renderer->blurProgram = blurProgram;
    BlurShaderInit(renderer, renderer->blurProgram, basePostShader, blurFragment);

    glUseProgram(renderer->basePostProgram->program);
    float gamma = 2.2f;
    glUniform1i(glGetUniformLocation(basePostProgram->program, "screenTexture"), 0);
    glUniform1i(glGetUniformLocation(basePostProgram->program, "bloomTexture"), 1);
    glUniform1f(glGetUniformLocation(basePostProgram->program, "gamma"), gamma);

    glUseProgram(renderer->blurProgram->program);
    glUniform1i(glGetUniformLocation(renderer->blurProgram->program, "screenTexture"), 0);

    glGenFramebuffers(1, &renderer->frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, renderer->frameBuffer);

    // Generate and configure the render texture
    glGenTextures(1, &renderer->postProcessingTexture);
    glBindTexture(GL_TEXTURE_2D, renderer->postProcessingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, INTERNAL_WIDTH, INTERNAL_HEIGHT, 
                 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Crisp pixels
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Attach texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                           GL_TEXTURE_2D, renderer->postProcessingTexture, 0);

    glGenTextures(1, &renderer->bloomTexture);
    glBindTexture(GL_TEXTURE_2D, renderer->bloomTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, INTERNAL_WIDTH, INTERNAL_HEIGHT, 
                 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Crisp pixels
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Attach texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 
                           GL_TEXTURE_2D, renderer->bloomTexture, 0);
     
    GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);


	glGenFramebuffers(2, renderer->pingPongBuffer);
	glGenTextures(2, renderer->pingPongTexture);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, renderer->pingPongBuffer[i]);
		glBindTexture(GL_TEXTURE_2D, renderer->pingPongTexture[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, INTERNAL_WIDTH, INTERNAL_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderer->pingPongTexture[i], 0);
	}

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
    const size_t TRANS_SIZE = 64 * 1024;
    
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

        PlatformRender(renderer, game->commands, game->renderCommandsCount, game->light);

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
        {{ x0, y0 }, {s0, t0}, { color[0], color[1], color[2], color[3] }},
        {{x1, y0 }, {s1, t0}, { color[0], color[1], color[2], color[3] }},
        {{x1, y1 }, {s1, t1}, { color[0], color[1], color[2], color[3] }},
        
        {{ x0, y0 }, {s0, t0}, { color[0], color[1], color[2], color[3] }},
        {{ x1, y1 }, {s1, t1}, { color[0], color[1], color[2], color[3] }},
        {{ x0, y1 }, {s0, t1}, { color[0], color[1], color[2], color[3] }}
    };
    
    glBindBuffer(GL_ARRAY_BUFFER, renderer->textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

static glm::vec2 GetAnchoredPosition(Anchor anchor, glm::vec2 offset, glm::vec2 screenSize) {
    glm::vec2 basePos = {};
    switch (anchor) {
        case Anchor::TOP_LEFT:
            basePos = {0, 0};
            break;
        case Anchor::BOTTOM_LEFT:
            basePos = {0, screenSize.y};
            offset.y = -offset.y; // Flip Y offset for bottom anchors
            break;
        case Anchor::BOTTOM_RIGHT:
            basePos = {screenSize.x, screenSize.y};
            offset.x = -offset.x;
            offset.y = -offset.y;
            break;
        case Anchor::TOP_RIGHT:
            basePos = {screenSize.x, 0};
            offset.x = -offset.x;
            break;
        case Anchor::CENTER:
            basePos = {screenSize.x * 0.5f, screenSize.y * 0.5f};
            break;
    }
    return basePos + offset;
}

static void RenderText(PlatformRenderer *renderer, const char* text, glm::vec2 pos, glm::vec4 color, Anchor anchor, glm::mat4 mvp) {

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    Font* usedFont = &renderer->fontUI;
    glUseProgram(renderer->textProgram->program);
    glBindVertexArray(renderer->textVAO);
    glBindTexture(GL_TEXTURE_2D, usedFont->texture_id);
    
    glUniformMatrix4fv(renderer->textProgram->uniformLocations[U_MVP], 1, GL_FALSE, &mvp[0][0]);

    glm::vec2 posOffset = GetAnchoredPosition(anchor, pos, { renderer->width, renderer->height });
    float x = posOffset.x;
    float y = posOffset.y;
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

void PlatformRender(PlatformRenderer* renderer, void* buffer, size_t size, LightSystem* system) {

    uint8_t* ptr = (uint8_t*)buffer;
    uint8_t* end = ptr + size;

    // TODO: Don't use a vector
    std::vector<RenderCommandDrawText*> textCommands;

    glBindFramebuffer(GL_FRAMEBUFFER, renderer->frameBuffer);
    glViewport(0, 0, INTERNAL_WIDTH, INTERNAL_HEIGHT);
    glEnable(GL_BLEND);

    glUseProgram(renderer->vertexProgram->program);
    glBindVertexArray(renderer->vertexVAO);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vertexVBO);

    AddLightsFrame(renderer->vertexProgram, system);

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

                glUniformMatrix4fv(renderer->vertexProgram->uniformLocations[U_MVP], 1, GL_FALSE, (const GLfloat*)&mvp);
                glUniformMatrix4fv(renderer->vertexProgram->uniformLocations[U_MODEL], 
                                   1, GL_FALSE, (const GLfloat*)&model);

                glBufferData(GL_ARRAY_BUFFER, d->vertexCount * sizeof(Vertex), verts, GL_DYNAMIC_DRAW);


                glDrawArrays(GL_TRIANGLES, 0, d->vertexCount);

                //glBindVertexArray(0);
            } break;
            case RENDER_CMD_DRAW_LOOP: {

                auto* d = (RenderCommandDrawTriangles*)ptr;
                void* verts = (uint8_t*)d + sizeof(RenderCommandDrawTriangles);
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(-d->pos.x, d->pos.y, 0.0f));

                glm::vec2 right(d->rotation.y, -d->rotation.x);  // perpendicular vector
                model[0][0] = right.x;  model[1][0] = right.y;
                model[0][1] = d->rotation.x; model[1][1] = d->rotation.y;

                //model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
                //glm::mat4 proj = glm::ortho(-renderer->ratio, renderer->ratio, -1.0f, 1.0f, 1.0f, -1.0f);
                glm::mat4 mvp = d->mvp * model;

                glUniformMatrix4fv(renderer->vertexProgram->uniformLocations[U_MVP], 1, GL_FALSE, (const GLfloat*)&mvp);
                glUniformMatrix4fv(renderer->vertexProgram->uniformLocations[U_MODEL], 
                                   1, GL_FALSE, (const GLfloat*)&model);

                glBufferData(GL_ARRAY_BUFFER, d->vertexCount * sizeof(Vertex), verts, GL_DYNAMIC_DRAW);

                glDrawArrays(GL_LINE_LOOP, 0, d->vertexCount);

            } break;
            case RENDER_CMD_BATCH_PARTICLES: {
                auto* d = (RenderCommandBatchParticles*)ptr;
                void* verts = (uint8_t*)d + sizeof(RenderCommandBatchParticles);
                glm::mat4 mvp = d->mvp;

                glEnable(GL_PROGRAM_POINT_SIZE);
                glUseProgram(renderer->particleProgram->program);
                glBindVertexArray(renderer->particleVAO);
                glBindBuffer(GL_ARRAY_BUFFER, renderer->particleVBO);
                glUniformMatrix4fv(
                    renderer->particleProgram->uniformLocations[U_MVP],
                    1,
                    GL_FALSE,
                    (float*)&mvp[0][0]
                );
                glBufferData(GL_ARRAY_BUFFER, d->vertexCount * sizeof(ParticleVertex), verts, GL_STREAM_DRAW);
                glDrawArrays(GL_POINTS, 0, d->vertexCount);
                glDisable(GL_PROGRAM_POINT_SIZE);

            } break;
            case RENDER_CMD_DRAW_TEXT: {
                auto* t = (RenderCommandDrawText*)ptr;
                textCommands.push_back(t);
            } break;
        }

        ptr += header->size; // move to next command
    }
    glBindVertexArray(0);
    glDisable(GL_BLEND);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, renderer->width, renderer->height);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(renderer->basePostProgram->program);
    glBindVertexArray(renderer->screenVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderer->postProcessingTexture);

    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, renderer->pingPongTexture[!horizontal]);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Render text to screen size
    glm::mat4 textMVP = glm::ortho(0.0f, (float)renderer->width, 
                                   (float)renderer->height, 0.0f, 
                                   -1.0f, 1.0f);
    for (auto* t : textCommands) {
        const char* str = (char*)t + sizeof(RenderCommandDrawText);
        RenderText(renderer, str, t->position, t->color, t->anchor, textMVP);
    }
}
