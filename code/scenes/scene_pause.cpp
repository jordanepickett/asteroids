#include <stdio.h>
#include "game.h"
#include "queues.cpp"
#include "platform.h"
#include "scenes/scene.h"
#include "systems.h"

// Forward declarations (file-local)
static void update(GameState* state, PlatformFrame* frame, PlatformMemory* memory);
static void onEnter(GameState* state);
static void onExit(GameState* state);

// Static scene definition
Scene ScenePause = {
    SCENE_PAUSE,
    SYS_INPUT | SYS_UI | SYS_RENDER,
    true,
    true,
    update,
    onEnter,
    onExit,
};

// ---- Implementation ----

static void onEnter(GameState* state) {
    printf("[Pause] Enter\n");
}

static void onExit(GameState* state) {
    printf("[Pause] Exit\n");
}

static void update(GameState* state, PlatformFrame* frame, PlatformMemory* memory) {
    if (WasPressed(frame->input.controllers[0].actionRight)) {
        printf("[Pause] back scene.\n");
        SceneStackPop(state);
    }
    // Input polling, menu selection, etc
    /*
    if (StartButtonPressed()) {
        SetScene(state, &Scene_Game);
    }
    */
}
