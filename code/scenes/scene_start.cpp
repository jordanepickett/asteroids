#include <stdio.h>
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

    EntityID start = CreateEntity2(state, {10, 20});
    AddButton(state, start, false, false);
    AddText(
        state,
        start,
        {1,0,1,1},
        CENTER,
        start,
        FIELD_UI
    );
}

static void onExit(GameState* state) {
    ClearGameSystems(state);
    printf("[Start] Exit\n");
}

static void update(GameState* state, PlatformFrame* frame, PlatformMemory* memory) {
    if (WasPressed(frame->input.controllers[0].actionDown)) {
        printf("[Start] next scene.\n");
        SceneStackPop(state);
        SceneStackPush(state, &SceneGame);
    }
    // Input polling, menu selection, etc
    /*
    if (StartButtonPressed()) {
        SetScene(state, &Scene_Game);
    }
    */
}
