#include "defs.h"
#include "entity.h"
#include "game.h"
#include "memory.h"
#include "queues.h"
#include "queues.cpp"
#include "render_commands.h"
#include "systems.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

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
            glm::vec2 pos = { x, y };

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

inline void PushText(GameState* state, MemoryArena* arena, glm::vec2 pos, glm::vec4 color, const char* str, Anchor anchor) {
    size_t len = strlen(str);
    size_t size = sizeof(RenderCommandDrawText) + len + 1;
    auto* cmd = (RenderCommandDrawText*)ArenaAlloc(arena, size);
    cmd->header.type = RENDER_CMD_DRAW_TEXT;
    cmd->header.size = (uint32_t)size;
    cmd->position = pos;
    cmd->color = color;
    cmd->length = (int)len;
    cmd->anchor = anchor;
    char* dst = (char*)cmd + sizeof(RenderCommandDrawText);
    memcpy(dst, str, len + 1);
}

static void PushTextf(GameState* state, MemoryArena* arena, glm::vec2 pos, glm::vec4 color, Anchor anchor, const char* fmt, ...) {
    char buffer[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    
    PushText(state, arena, pos, color, buffer, anchor);
}

inline void PushTrianges2(
    glm::mat4 mvp,
    MemoryArena *memory,
    Vertex *verts,
    int vertexCount,
    glm::vec2 pos,
    glm::vec2 rot
) {

    size_t size = sizeof(RenderCommandDrawTriangles) + vertexCount * sizeof(Vertex); // assuming simple 2D verts
    auto* drawCmd = (RenderCommandDrawTriangles*)ArenaAlloc(memory, size);
    drawCmd->header.type = RENDER_CMD_DRAW_TRIANGLES;
    drawCmd->header.size = (uint32_t)size;
    drawCmd->mvp = mvp;
    drawCmd->vertexCount = vertexCount;
    drawCmd->pos = pos;
    drawCmd->rotation = rot;
    void* dst = (uint8_t*)drawCmd + sizeof(RenderCommandDrawTriangles);
    memcpy(dst, verts, vertexCount * sizeof(Vertex));
}

inline void PushLoop2(
    glm::mat4 mvp,
    MemoryArena *memory,
    Vertex *verts,
    int vertexCount,
    glm::vec2 pos,
    glm::vec2 rot
) {

    size_t size = sizeof(RenderCommandDrawTriangles) + vertexCount * sizeof(Vertex); // assuming simple 2D verts
    auto* drawCmd = (RenderCommandDrawTriangles*)ArenaAlloc(memory, size);
    drawCmd->header.type = RENDER_CMD_DRAW_LOOP;
    drawCmd->header.size = (uint32_t)size;
    drawCmd->pos = pos;
    drawCmd->rotation = rot;
    drawCmd->mvp = mvp;
    drawCmd->vertexCount = vertexCount;
    void* dst = (uint8_t*)drawCmd + sizeof(RenderCommandDrawTriangles);
    memcpy(dst, verts, vertexCount * sizeof(Vertex));
}

inline void PushTrianges(
    GameState *state,
    MemoryArena *memory,
    Vertex *verts,
    int vertexCount,
    Entity* entity
) {

    glm::mat4 mvp = state->camera.projection * state->camera.view;
    size_t size = sizeof(RenderCommandDrawTriangles) + vertexCount * sizeof(Vertex); // assuming simple 2D verts
    auto* drawCmd = (RenderCommandDrawTriangles*)ArenaAlloc(memory, size);
    drawCmd->header.type = RENDER_CMD_DRAW_TRIANGLES;
    drawCmd->header.size = (uint32_t)size;
    drawCmd->mvp = mvp;
    drawCmd->vertexCount = vertexCount;
    drawCmd->pos = entity->transform.position;
    void* dst = (uint8_t*)drawCmd + sizeof(RenderCommandDrawTriangles);
    memcpy(dst, verts, vertexCount * sizeof(Vertex));
}

inline void PushLoop(
    GameState *state,
    MemoryArena *memory,
    Vertex *verts,
    int vertexCount,
    Entity* entity
) {

    glm::mat4 mvp = state->camera.projection * state->camera.view;
    size_t size = sizeof(RenderCommandDrawTriangles) + vertexCount * sizeof(Vertex); // assuming simple 2D verts
    auto* drawCmd = (RenderCommandDrawTriangles*)ArenaAlloc(memory, size);
    drawCmd->header.type = RENDER_CMD_DRAW_LOOP;
    drawCmd->header.size = (uint32_t)size;
    drawCmd->pos = entity->transform.position;
    drawCmd->mvp = mvp;
    drawCmd->vertexCount = vertexCount;
    drawCmd->rotation = entity->transform.rotation;
    void* dst = (uint8_t*)drawCmd + sizeof(RenderCommandDrawTriangles);
    memcpy(dst, verts, 4 * sizeof(Vertex));
}

inline void UpdateCamera(GameState *state, Entity *entity) {
    if(!state->camera.isLocked) {
        state->camera.position = glm::vec3(-entity->transform.position.x, entity->transform.position.y, 1.0f);
        state->camera.view = glm::lookAt(
            state->camera.position,             // eye
            glm::vec3(-entity->transform.position.x, entity->transform.position.y, 0.0f),        // target
            glm::vec3(0.0f, 1.0f, 0.0f)         // up
        );
    }
}

inline void WrapSystem(GameState* state) {
    float left   = -20.0f;
    float right  =  20.0f;
    float bottom = -20.0f;
    float top    =  20.0f;

    MovementSystem* movementSystem = state->movement;
    for(int i = 0; i < state->entitiesReg->count; i++) {
        if(!(state->entitiesReg->comp[i] & COMP_MOVEMENT)) {
            continue;
        }
        glm::vec2 pos = movementSystem->pos[i];
        if (pos.x > right)       pos.x = left;
        else if (pos.x < left)   pos.x = right;

        if (pos.y > top)         pos.y = bottom;
        else if (pos.y < bottom) pos.y = top;

        movementSystem->pos[i] = pos;
    }

}

inline void WrapEntityPosition(Entity *entity) {
    float left   = -20.0f;
    float right  =  20.0f;
    float bottom = -20.0f;
    float top    =  20.0f;

    if (entity->transform.position.x > right)       entity->transform.position.x = left;
    else if (entity->transform.position.x < left)   entity->transform.position.x = right;

    if (entity->transform.position.y > top)         entity->transform.position.y = bottom;
    else if (entity->transform.position.y < bottom) entity->transform.position.y = top;
}

Entity* CreateEntity(GameState* state, EntityType entityType) {
    for(int i = 0; i < MAX_ENTITIES; i++) {
        if(!state->entities[i].isActive) {
            Entity* e = &state->entities[i];
            *e = {};
            e->isActive = true;
            e->type = entityType;
            e->transform = {
                glm::vec2(0.0f, 0.0f),
                glm::vec2(0.0f, 1.0f),
                glm::vec2(1.0f, 1.0f),
                glm::vec2(0.0f, 0.0f)
            };
            return e;
        }
    }

    return nullptr;
}

void DestoryEntity(Entity *entity) {
    entity->isActive = false;
}

void GameInit(GameState *state, PlatformAPI *platform, PlatformMemory *memory) {
    
    // Systems
    state->entitiesReg = (EntityRegistry*)ArenaAlloc(&memory->permanent, sizeof(EntityRegistry));
    state->render = (RenderSystem*)ArenaAlloc(&memory->permanent, sizeof(RenderSystem));
    state->movement = (MovementSystem*)ArenaAlloc(&memory->permanent, sizeof(MovementSystem));
    state->health = (HealthSystem*)ArenaAlloc(&memory->permanent, sizeof(HealthSystem));
    state->damage = (DamageSystem*)ArenaAlloc(&memory->permanent, sizeof(DamageSystem));
    state->cameraSys = (CameraSystem*)ArenaAlloc(&memory->permanent, sizeof(CameraSystem));
    state->playerInput = (PlayerInputSystem*)ArenaAlloc(&memory->permanent, sizeof(PlayerInputSystem));
    state->fireMissile = (FireMissleSystem*)ArenaAlloc(&memory->permanent, sizeof(FireMissleSystem));
    state->lifetime = (LifeTimeSystem*)ArenaAlloc(&memory->permanent, sizeof(LifeTimeSystem));
    state->floatable = (FloatableSystem*)ArenaAlloc(&memory->permanent, sizeof(FloatableSystem));
    state->collision = (CollisionSystem*)ArenaAlloc(&memory->permanent, sizeof(CollisionSystem));

    // Queues
    state->collisions = (CollisionQueue*)ArenaAlloc(&memory->permanent, sizeof(CollisionQueue));
    state->projectile = (ProjectileQueue*)ArenaAlloc(&memory->permanent, sizeof(ProjectileQueue));


    // Systems 0
    memset(state->entitiesReg, 0, sizeof(EntityRegistry));
    memset(state->render, 0, sizeof(RenderSystem));
    memset(state->movement, 0, sizeof(MovementSystem));
    memset(state->health, 0, sizeof(HealthSystem));
    memset(state->damage, 0, sizeof(DamageSystem));
    memset(state->cameraSys, 0, sizeof(CameraSystem));
    memset(state->playerInput, 0, sizeof(PlayerInputSystem));
    memset(state->fireMissile, 0, sizeof(FireMissleSystem));
    memset(state->lifetime, 0, sizeof(LifeTimeSystem));
    memset(state->floatable, 0, sizeof(FloatableSystem));
    memset(state->collision, 0, sizeof(CollisionSystem));

    // Queues 0
    memset(state->collisions, 0, sizeof(CollisionQueue));
    memset(state->projectile, 0, sizeof(ProjectileQueue));

    //EntityRegistryInit(state->entitiesReg);
    //LifetimeSystemInit(state->lifetime);
    //MovementSystemInit(state->movement);
    //HealthSystemInit(state->health);
    //DamageSystemInit(state->damage);
    //RenderSystemInit(state->render);
    //CollisionSystemInit(state->collision);
    //RenderQueueInit(g->render);
    //CollisionQueueInit(state->collisions);

    //TODO move to scenes
    EntityID player = CreateEntity2(state);
    AddTag(state, player, TAG_PLAYER);
    glm::vec2 zero = { 0, 0};
    glm::vec2 rot = { 0, 1};
    AddMovement(state, player, zero, rot, zero);
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
}

void GameUpdate(GameState *state, PlatformFrame *frame, PlatformMemory *memory) {

    asteroidSpawnTimer -= frame->deltaTime;
    if (asteroidSpawnTimer <= 0.0f) {
        TrySpawnAsteroid(state);
        asteroidSpawnTimer = 3.0f; // spawn every ~3 seconds
    }

    UpdateEntities(state, frame);
    //GameRender(state, memory, frame);
}

void UpdateEntities(GameState *state, PlatformFrame *frame) {
    //printf("entity count: %i\n", state->entitiesReg->count);

    // Inputs
    PlayerInputUpdate(state, frame);
    FireMissleUpdate(state, frame);
    // Queue processors
    ProcessProjectileFire(state);

    // Systems
    MovementUpdate(state, frame);
    LifeTimeUpdate(state, frame);

    CollisionUpdate(state, state->collisions);
    ProcessCollisions(state);

    if(state->camera.isLocked) {
        WrapSystem(state);
    }

    // Cleanup
    CleanupDeadEntities(state);

}

void GameRender(GameState *state, PlatformMemory *memory, PlatformFrame* frame) {
    ArenaReset(&memory->transient);

    // Clear
    auto* cmd = (RenderCommandClear*)ArenaAlloc(&memory->transient, sizeof(RenderCommandClear));
    cmd->header.type = RENDER_CMD_CLEAR;
    cmd->header.size = sizeof(RenderCommandClear);
    cmd->color = {0.f, 0.f, 0.f};

    MovementSystem *movementSystem = state->movement;
    RenderSystem *renderSystem = state->render;
    CameraSystem *cameraSystem = state->cameraSys;

    glm::mat4 cameraMvp;
    for(int i = 0; i < state->entitiesReg->count; i++) {
        if((state->entitiesReg->comp[i] & (COMP_CAMERA))) {
            if(cameraSystem->isActive[i]) {
                cameraMvp = cameraSystem->projection[i] * cameraSystem->view[i];
            }
        }
    }

    for(int i = 0; i < state->entitiesReg->count; i++) {
        if((state->entitiesReg->comp[i] & (COMP_RENDER | COMP_MOVEMENT))) {

            glm::vec2 pos = {0,0};
            glm::vec2 rot = {0,1};
            pos = movementSystem->pos[i];
            rot = movementSystem->rot[i];
            Vertex *verts = renderSystem->verts[i];
            int count = renderSystem->vertCount[i];

            if(count == 3) {
                PushTrianges2(cameraMvp, &memory->transient, verts, count, pos, rot);
            } else {
                PushLoop2(cameraMvp, &memory->transient, verts, count, pos, rot);
            }
        }
    }

    // TODO: Text
    PushTextf(state, &memory->transient, {10, 20}, {1,1,1,1}, TOP_LEFT, "FPS: %f", (1.0f / frame->deltaTime));
    PushTextf(state, &memory->transient, {10, 20}, {1,1,1,1}, BOTTOM_LEFT, "HEALTH: %f", 100.0f, BOTTOM_RIGHT);


    state->commands = memory->transient.base;
    state->renderCommandsCount = memory->transient.used;
}

