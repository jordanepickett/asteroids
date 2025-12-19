#include <stdio.h>
#include "queues.cpp"
#include "platform.h"
#include "scenes/scene_start.h"

// Forward declarations (file-local)
static void update(GameState* state, PlatformFrame* frame, PlatformMemory* memory);
static void onEnter(GameState* state);
static void onExit(GameState* state);

// Static scene definition
Scene SceneGame = {
    SCENE_GAME,
    update,
    onEnter,
    onExit,
};

// ---- Implementation ----

static void onEnter(GameState* state) {
    printf("[Game] Enter\n");
    EntityID player = CreateEntity2(state);
    AddTag(state, player, TAG_PLAYER);
    glm::vec2 zero = { 0, 0};
    glm::vec2 rot = { 0, 1};
    AddMovement(state, player, zero, rot, zero);
    AddHealth(state, player, 3.0f);
    AddRender(state, player, SHIP, 3);
    AddPlayerInput(state, player);
    AddFireMissleSystem(state, player);
    AddCollision(state, player, 1.0f);

    EntityID camera = CreateEntity2(state);
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::mat4 lookAt = glm::lookAt(
        cameraPos,
        glm::vec3(0.0f, 0.0f, 0.0f),        // target
        glm::vec3(0.0f, 1.0f, 0.0f)         // up
    );
    AddCamera(state, camera, lookAt, cameraPos, true, true);

    EntityID healthText = CreateEntity2(state);
    AddText(
        state,
        healthText,
        {10, 20},
        {1,0,1,1},
        BOTTOM_LEFT,
        player,
        FIELD_HEALTH
    );

    EntityID light = CreateEntity2(state);
    AddLight(state, light, {1, 1}, {1, 0, 0}, 15.0f, 50.0f);
    AddMovement(state, light, {1, 0}, {0, 1}, {0, 0});
    AddRender(state, light, MISSLE, 4);

    EntityID light2= CreateEntity2(state);
    AddLight(state, light2, {15, 15}, {0, 0, 1}, 15.0f, 50.0f);
    AddMovement(state, light2, {15, 15}, {0, 1}, {0, 0});
    AddRender(state, light2, MISSLE, 4);

    EntityID emitter = CreateEntity2(state);
    AddMovement(state, emitter, {-10, 0}, {0, 1}, {10, 0});
    /*
    AddEmitter(
        state,
        emitter,
        {-10, 0},
        {0, 0},
        {5, 5},
        25,
        0,
        0.5,
        {1, 1, 0, 1},
        {1, 0, 0, 1},
        1,
        1
    );

    EntityID emitter2 = CreateEntity2(state);
    AddMovement(state, emitter2, {10, 0}, {0, 1}, {-10, 0});
    AddEmitter(
        state,
        emitter2,
        {-10, 0},
        {0, 0},
        {5, 5},
        25,
        0,
        0.5,
        {1, 0.3, 0, 1},
        {1, 0, 0, 1},
        1,
        1
    );
    */

}

static void onExit(GameState* state) {
    ClearGameSystems(state);
    printf("[Game] Exit\n");
}

static void update(GameState* state, PlatformFrame* frame, PlatformMemory* memory) {
    if (WasPressed(frame->input.controllers[0].actionRight)) {
        printf("[Game] next scene.\n");
        onExit(state);
        state->sceneStack.scenes[0] = &SceneStart;
        state->sceneStack.scenes[0]->onEnter(state);
    }
    // Input polling, menu selection, etc
    /*
    if (StartButtonPressed()) {
        SetScene(state, &Scene_Game);
    }
    */
}
