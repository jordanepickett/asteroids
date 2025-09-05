#include "game.h"
#include "memory.h"
#include "platform.h"
#include "render_commands.h"
#include <cstdio>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

inline void PushText(MemoryArena* arena, glm::vec2 pos, glm::vec4 color, const char* str) {
    size_t len = strlen(str);
    size_t size = sizeof(RenderCommandDrawText) + len + 1;
    auto* cmd = (RenderCommandDrawText*)ArenaAlloc(arena, size);
    cmd->header.type = RENDER_CMD_DRAW_TEXT;
    cmd->header.size = (uint32_t)size;
    cmd->position = pos;
    cmd->color = color;
    cmd->length = (int)len;
    char* dst = (char*)cmd + sizeof(RenderCommandDrawText);
    memcpy(dst, str, len + 1);
}

inline void PushTrianges(
    GameState *state,
    MemoryArena *memory,
    Vertex *verts,
    int vertexCount
) {

    glm::mat4 mvp = state->camera.projection * state->camera.view;
    size_t size = sizeof(RenderCommandDrawTriangles) + 3 * sizeof(Vertex); // assuming simple 2D verts
    auto* drawCmd = (RenderCommandDrawTriangles*)ArenaAlloc(memory, size);
    drawCmd->header.type = RENDER_CMD_DRAW_TRIANGLES;
    drawCmd->header.size = (uint32_t)size;
    drawCmd->mvp = mvp;
    drawCmd->vertexCount = 3;
    drawCmd->pos = state->player.pos;
    void* dst = (uint8_t*)drawCmd + sizeof(RenderCommandDrawTriangles);
    memcpy(dst, verts, 3 * sizeof(Vertex));
}

inline void PushLoop(
    GameState *state,
    MemoryArena *memory,
    Vertex *verts,
    int vertexCount
) {

    glm::mat4 mvp = state->camera.projection * state->camera.view;
    size_t size = sizeof(RenderCommandDrawTriangles) + 4 * sizeof(Vertex); // assuming simple 2D verts
    auto* drawCmd = (RenderCommandDrawTriangles*)ArenaAlloc(memory, size);
    drawCmd->header.type = RENDER_CMD_DRAW_LOOP;
    drawCmd->header.size = (uint32_t)size;
    drawCmd->pos = state->player.pos;
    drawCmd->rotation = state->player.rotation;
    drawCmd->mvp = mvp;
    drawCmd->vertexCount = 4;
    void* dst = (uint8_t*)drawCmd + sizeof(RenderCommandDrawTriangles);
    memcpy(dst, verts, 4 * sizeof(Vertex));
}

inline void UpdateCamera(GameState *state) {
    if(!state->camera.isLocked) {
        state->camera.position = glm::vec3(state->player.pos.x, state->player.pos.y, 1.0f);
        state->camera.view = glm::lookAt(
            state->camera.position,             // eye
            glm::vec3(state->player.pos.x, state->player.pos.y, 0.0f),        // target
            glm::vec3(0.0f, 1.0f, 0.0f)         // up
        );
    }
}

void GameInit(GameState *state, PlatformAPI *platform, PlatformMemory *memory) {
    float left   = -20.0f;
    float right  =  20.0f;
    float bottom = -20.0f;
    float top    =  20.0f;
    float nearZ  = -1.0f;
    float farZ   =  1.0f;

    state->camera.projection = glm::ortho(left, right, bottom, top, nearZ, farZ);

    // Place camera at z=1 looking at the origin
    state->camera.position = glm::vec3(0.0f, 0.0f, 1.0f);
    state->camera.view = glm::lookAt(
        state->camera.position,             // eye
        glm::vec3(0.0f, 0.0f, 0.0f),        // target
        glm::vec3(0.0f, 1.0f, 0.0f)         // up
    );
    state->camera.isLocked = true;
    state->player.rotation = glm::vec2(0.0f, 1.0f);
}

void GameUpdate(GameState *state, PlatformFrame *frame, PlatformMemory *memory) {
    float rightBurst = 0;
    float leftBurst = 0;
    float lx = 0;
    float ly = 0;
    if(frame->input.controllers[0].actionDown.endedDown) {
        printf("A Button Pressed. \n");
        rightBurst = 1;
    } else {
        rightBurst = 0;
    }
    if(frame->input.controllers[0].moveDown.endedDown) {
        //state->player.pos.y -= 0.02f;
    }
    if(frame->input.controllers[0].moveUp.endedDown) {
        //state->player.pos.y += 0.02f;
    }
    if(frame->input.controllers[0].moveLeft.endedDown) {
        //state->player.pos.x -= 0.02f;
    }
    if(frame->input.controllers[0].moveRight.endedDown) {
        //state->player.pos.x += 0.02f;
    }
    
    lx = frame->input.controllers[0].stickAverageX;
    ly = frame->input.controllers[0].stickAverageY;
    //printf("X: %f. \n", state->playPos.x);
    //printf("Y: %f. \n", state->playPos.y);


    float rotSpeed  = 0.02f;

    float acceleration = 0.0002f; // thrust strength
    float maxSpeed     = 0.2f;  // max velocity
    float damping      = 0.98f;
    float rotLerp   = 0.15f;

    // Per-frame input
    glm::vec2 input(lx, ly);

    if (glm::length(input) > 0.15f) {
        // Normalize stick input
        input = glm::normalize(input);

        // Correct Y axis (gamepad up is usually negative)
        input.y = -input.y;
        input.x = -input.x;

        // Smoothly rotate facing vector toward input
        state->player.rotation = glm::normalize(glm::mix(state->player.rotation, input, rotLerp));
    }

    if(rightBurst) {
        state->player.velocity += state->player.rotation * acceleration;
    }

    if (glm::length(state->player.velocity) > maxSpeed) {
        state->player.velocity = glm::normalize(state->player.velocity) * maxSpeed;
    }

    state->player.velocity *= damping;

    state->player.pos += state->player.velocity;
    printf("Vel: %f %f \n", state->player.velocity.x, state->player.velocity.y);
    //printf("Pos: %f %f \n", state->player.pos.x, state->player.pos.y);

    //glm::mat4 model = glm::mat4(1.0f); // identity
    //model = glm::translate(model, glm::vec3(-state->player.pos.x, state->player->pos.y, 0.0f));

    //glm::vec2 right(state->player.rotation.y, -state->player.rotation.x);  // perpendicular vector
    //model[0][0] = right.x;  model[1][0] = right.y;
    //model[0][1] = facing.x; model[1][1] = facing.y;
    //model = glm::rotate(model, test, glm::vec3(0.0f, 0.0f, 1.0f));

}

void GameRender(GameState *state, PlatformMemory *memory) {
    ArenaReset(&memory->transient);

    // Clear
    auto* cmd = (RenderCommandClear*)ArenaAlloc(&memory->transient, sizeof(RenderCommandClear));
    cmd->header.type = RENDER_CMD_CLEAR;
    cmd->header.size = sizeof(RenderCommandClear);
    cmd->color = {0.f, 0.f, 0.f};

    {
        Vertex verts[3] = {
            {{ 0.0f,  1.0f}, {1.f, 0.f, 0.f}},
            {{-1.0f, -1.0f}, {0.f, 1.f, 0.f}},
            {{ 1.0f, -1.0f}, {0.f, 0.f, 1.f}},
        };
        PushTrianges(state, &memory->transient, verts, 3);
    }

    {
        Vertex verts[4] = {
            {{ 0.0f,  0.0f}, {1.f, 0.f, 0.f}},
            {{ 0.0f, -1.0f}, {0.f, 1.f, 0.f}},
            {{ 1.0f, -1.0f}, {0.f, 0.f, 1.f}},
            {{ 1.0f,  0.0f}, {0.f, 0.f, 1.f}},
        };
        PushLoop(state, &memory->transient, verts, 4);
    }

    // TODO: Text
    //PushText(&memory->transient, {20, 20}, {1,1,1,1}, "Health: 100");

    state->commands = memory->transient.base;
    state->renderCommandsCount = memory->transient.used;
}

