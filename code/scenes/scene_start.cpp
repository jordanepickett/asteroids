#include <stdio.h>
#include "scene_start.h"
#include "game.h"
#include "scene_game.h"
#include "queues.cpp"
#include "platform.h"

// Forward declarations (file-local)
static void update(GameState* state, PlatformFrame* frame, PlatformMemory* memory);
static void onEnter(GameState* state);
static void onExit(GameState* state);

// Static scene definition
Scene SceneStart = {
    SCENE_START,
    update,
    onEnter,
    onExit,
};

// ---- Implementation ----

static void onEnter(GameState* state) {
    printf("[Start] Enter\n");
}

static void onExit(GameState* state) {
    ClearGameSystems(state);
    printf("[Start] Exit\n");
}

static void update(GameState* state, PlatformFrame* frame, PlatformMemory* memory) {
    if (WasPressed(frame->input.controllers[0].actionDown)) {
        printf("[Start] next scene.\n");
        onExit(state);
        state->sceneStack.scenes[0] = &SceneGame;
        state->sceneStack.scenes[0]->onEnter(state);
    }
    // Input polling, menu selection, etc
    /*
    if (StartButtonPressed()) {
        SetScene(state, &Scene_Game);
    }
    */
}
