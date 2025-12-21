#include <stdio.h>
#include "queues.cpp"
#include "platform.h"
#include "scenes/scene.h"
#include "scenes/scene_start.h"
#include "scenes/scene_pause.h"
#include "systems.h"

// Forward declarations (file-local)
static void update(GameState* state, PlatformFrame* frame, PlatformMemory* memory);
static void onEnter(GameState* state);
static void onExit(GameState* state);

// Static scene definition
Scene SceneGame = {
    SCENE_GAME,
    SYS_INPUT | SYS_UI | SYS_MOVE | SYS_RENDER | SYS_PARTICLES,
    true,
    true,
    update,
    onEnter,
    onExit,
};

float asteroidSpawnTimer = 0.0f;

static int CountAsteroids(GameState* state) {
    int count = 0;
    for(int i = 0; i < state->entitiesReg->count; i++) {
        if(state->entitiesReg->comp[i] & COMP_FLOATABLE) {
            count++;
        }
    }
    return count;
}

static void TrySpawnAsteroid(GameState* state) {
    int amount = CountAsteroids(state);
    //printf("AMOUNT: %i \n", amount);
    if (amount >= MAX_FLOATABLES) return;

    while(amount < MAX_FLOATABLES) {
        EntityID a = CreateEntity2(state);
        if (a) {
            // random position at edges of screen
            float x = (rand() % 2 == 0) ? 0.0f : 640;
            float y = (float)(rand() % 480);
            glm::vec2 pos = { x + amount, y };

            // random velocity (pick a random direction + speed)
            float angle = (float)rand() / RAND_MAX * 2.0f * 3.14159f;
            float speed = 5.0f + (rand() % 5); // 50–150 px/sec
            float velX = cosf(angle) * speed;
            float velY  = sinf(angle) * speed;
            glm::vec2 vel = { velX, velY };
            glm::vec2 rot = glm::normalize(vel);
            AddMovement(state, a, pos, rot, vel);
            AddTag(state, a, TAG_ASTEROID);
            AddDamage(state, a, 1.0f, TAG_MISSLE);
            AddRender(state, a, ASTEROID, 8);
            AddFloatable(state, a);
            AddCollision(state, a, 0.5f);

            //a->radius = 20.0f + (rand() % 15); // random asteroid size
        }
        amount++;
    }

}

// ---- Implementation ----

static void onEnter(GameState* state) {
    printf("[Game] Enter\n");
    asteroidSpawnTimer = 0;
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
        SceneStackPop(state);
        SceneStackPush(state, &SceneStart);
    }
    if (WasPressed(frame->input.controllers[0].actionUp)) {
        printf("[Game] pause.\n");
        SceneStackPush(state, &ScenePause);
    }
    asteroidSpawnTimer -= frame->deltaTime;
    if (asteroidSpawnTimer <= 0.0f) {
        TrySpawnAsteroid(state);
        asteroidSpawnTimer = 3.0f; // spawn every ~3 seconds
    }
}
