#include "game.h"
#include "memory.h"
#include "platform.h"
#include "render_commands.h"
#include <cstdio>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

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

inline void PushTrianges(MemoryArena *memory, Vertex *verts, int vertexCount) {

    //glm::mat4 mvp = state->camera.projection * state->camera.view;
    //glm::mat4 proj = glm::ortho(-platform->ratio, platform->ratio, -1.0f, 1.0f, 1.0f, -1.0f);
    size_t size = sizeof(RenderCommandDrawTriangles) + 3 * sizeof(Vertex); // assuming simple 2D verts
    auto* drawCmd = (RenderCommandDrawTriangles*)ArenaAlloc(memory, size);
    drawCmd->header.type = RENDER_CMD_DRAW_TRIANGLES;
    drawCmd->header.size = (uint32_t)size;
    //drawCmd->mvp = mvp;
    drawCmd->vertexCount = 3;
    void* dst = (uint8_t*)drawCmd + sizeof(RenderCommandDrawTriangles);
    memcpy(dst, verts, 3 * sizeof(Vertex));
}

void GameInit(GameState *state, PlatformAPI *platform, PlatformMemory *memory) {
    float left   = -100.0f;
    float right  =  100.0f;
    float bottom = -100.0f;
    float top    =  100.0f;
    float nearZ  = -1.0f;
    float farZ   =  1.0f;

    //state->camera.projection = glm::ortho(left, right, bottom, top, nearZ, farZ);

    // Place camera at z=1 looking at the origin
    state->camera.position = glm::vec3(0.0f, 0.0f, 1.0f);
    //state->camera.view = glm::lookAt(
    //    state->camera.position,             // eye
    //    glm::vec3(0.0f, 0.0f, 0.0f),        // target
    //    glm::vec3(0.0f, 1.0f, 0.0f)         // up
    //);
}

void GameUpdate(GameState *state, PlatformFrame *frame, PlatformMemory *memory) {
    if(frame->input.controllers[0].actionDown.endedDown) {
        printf("A Button Pressed. \n");
    }
    if(frame->input.controllers[0].moveDown.endedDown) {
        printf("Move Down. \n");
    }
    if(frame->input.controllers[0].moveUp.endedDown) {
        printf("Move Up. \n");
    }
    if(frame->input.controllers[0].moveLeft.endedDown) {
        printf("Move Left. \n");
    }
    if(frame->input.controllers[0].moveRight.endedDown) {
        printf("Move Right. \n");
    }

}

void GameRender(GameState *state, PlatformMemory *memory) {
    ArenaReset(&memory->transient);

    // Clear
    auto* cmd = (RenderCommandClear*)ArenaAlloc(&memory->transient, sizeof(RenderCommandClear));
    cmd->header.type = RENDER_CMD_CLEAR;
    cmd->header.size = sizeof(RenderCommandClear);
    cmd->color = {0.f, 0.f, 0.f};

    Vertex verts[3] = {
        {{ 0.0f,  0.2f}, {1.f, 0.f, 0.f}},
        {{-0.2f, -0.2f}, {0.f, 1.f, 0.f}},
        {{ 0.2f, -0.2f}, {0.f, 0.f, 1.f}},
    };
    //glm::mat4 mvp = state->camera.projection * state->camera.view;
    //glm::mat4 proj = glm::ortho(-platform->ratio, platform->ratio, -1.0f, 1.0f, 1.0f, -1.0f);
    PushTrianges(&memory->transient, verts, 3);

    // TODO: Text
    //PushText(&memory->transient, {20, 20}, {1,1,1,1}, "Health: 100");

    state->commands = memory->transient.base;
    state->renderCommandsCount = memory->transient.used;
}

