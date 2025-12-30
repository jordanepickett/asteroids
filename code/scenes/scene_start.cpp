#include <stdio.h>
#include "defs.h"
#include "queues.cpp"
#include "scene_start.h"
#include "game.h"
#include "scene_game.h"
#include "platform.h"
#include "scenes/scene.h"
#include "systems.h"

// Forward declarations (file-local)
static void update(GameState* state, PlatformFrame* frame, PlatformMemory* memory);
static void onEnter(GameState* state);
static void onExit(GameState* state);

// Static scene definition
Scene SceneStart = {
    SCENE_START,
    {},
    0,
    SYS_INPUT | SYS_UI | SYS_RENDER,
    true,
    true,
    update,
    onEnter,
    onExit,
};

// ---- Implementation ----

static void onEnter(GameState* state) {
    printf("[Start] Enter\n");
    EntityID camera = CreateEntity2(state);
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::mat4 lookAt = glm::lookAt(
        cameraPos,
        glm::vec3(0.0f, 0.0f, 0.0f),        // target
        glm::vec3(0.0f, 1.0f, 0.0f)         // up
    );
    AddCamera(state, camera, lookAt, cameraPos, true, true);

    // Utilize anchors for raw position with the transform as the offset from that (For buttons -> children (text))
    EntityID start = CreateEntity2(state, {0, -100});
    AddButton(state, start, { 150, 30}, true, true);
    AddText(
        state,
        start,
        {1,1,1,1},
        CENTER,
        start,
        FIELD_START_GAME
    );
    EntityID settings = CreateEntity2(state, {0, -50});
    AddButton(state, settings, { 150, 30 }, false, true);
    AddText(
        state,
        settings,
        {1,1,1,1},
        CENTER,
        settings,
        FIELD_OPEN_SETTINGS
    );

    EntityID exit = CreateEntity2(state, {0, 0});
    AddButton(state, exit, { 150, 30 }, false, true);
    AddText(
        state,
        exit,
        {1,1,1,1},
        CENTER,
        exit,
        FIELD_OPEN_SETTINGS
    );

    state->buttons->selectedButton = start;
}

static void onExit(GameState* state) {
    ClearGameSystems(state);
    printf("[Start] Exit\n");
}

static void update(GameState* state, PlatformFrame* frame, PlatformMemory* memory) {
    if(WasPressed(frame->input.controllers[0].moveDown)) {
        printf("Move down\n");
        EntityID newButton = state->buttons->selectedButton + 1;
        printf("new button: %i\n", newButton);
        if(!state->buttons->present[newButton]) {
            for(int i = 0; i < state->entitiesReg->count; i++) {
                if(state->buttons->present[i]) {
                    state->buttons->selectedButton = i;
                    break;
                }
            }
        } else {
            state->buttons->selectedButton = newButton;
        }
    }

    if(WasPressed(frame->input.controllers[0].moveUp)) {
        printf("Move up\n");
        EntityID newButton = state->buttons->selectedButton - 1;
        printf("new button: %i\n", newButton);
        if(!state->buttons->present[newButton]) {
            for(int i = state->entitiesReg->count; i > 0; i--) {
                if(state->buttons->present[i]) {
                    state->buttons->selectedButton = i;
                    break;
                }
            }
        } else {
            state->buttons->selectedButton = newButton;
        }
    }

    if(WasPressed(frame->input.controllers[0].actionDown) && (state->buttons->selectedButton == 1)) {
        SceneStackPop(state);
        SceneStackPush(state, &SceneGame);
    }

}
